/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   StressTest.cpp
 * Author: tim
 *
 * Created on 5. Mai 2017, 12:11
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <ctime>

#include "../StoreManager.h"
#include "../Sensor.h"
#include "../Server.h"
#include "../HttpServer.h"
#include "../Producer.h"

#define DEFAULT_ADDRESS "127.0.0.1"
#define STORE_COUNT 2
#define STORE_ITEMS "Cheese", "Bread", "Milk", "Juice"
#define DEFAULT_PORT 27015

using namespace std;

sockaddr_in addr;
string lastMessage;
unsigned int receivedMessageCount = 0;

class TestCallback : public MosqCallback {
public:

    void on_mosqEvent(const char* channel, const void* msg) {
        cout << "Callback called" << endl;
        
        if(!strcmp(channel, ORDER_CHANNEL))
            orders++;
        else if(strstr(channel, OFFER_PREFIX))
            offer++;
    }
    
    unsigned getReceivedOrders() const { return orders; }
    unsigned getReceivedOffes() const { return offer; }
    
private:
    unsigned int orders = 0;
    unsigned int offer = 0;

};

string generateHttpResponse(RESTManager manager);

void serverReceivedMessage(string message) {
    receivedMessageCount++;
    lastMessage = message;
    cout << "Message Received" << endl;
}

bool loadTest() {
    cout << endl << "Testing behavior under heavy load" << endl;
    receivedMessageCount = 0;
    Sensor* sensors[1000];
    thread* threads[1000];
    for(int i = 0; i < 1000; i++) {
        sensors[i] = new Sensor(to_string(i), addr, 10, -1);
        threads[i] = new thread(&Sensor::send, sensors[i]);
    }
    
    for(int i = 0; i < 1000; i++) {
        threads[i]->join();
        delete sensors[i];
    }
    
    sleep(1);
    
    if(receivedMessageCount < (1000*10)*0,95) {
        cout << "Server overloaded, only " << receivedMessageCount << " of 10000 received, test failed" << endl;
        //exit(EXIT_FAILURE);
        return false;
    }
    
    cout << "Test successful" << endl;
    return true;
}

bool lengthTest() {
    cout << endl << "Testing behaviour when sending of long messages" << endl;
    string longString = "";
    for(int i = 0; i < 10000; i++)
        longString += "A";
    
    Sensor * sensor = new Sensor(longString, addr, 1, -1);
    
    thread sensorThread(&Sensor::send, sensor);
    sensorThread.join();
    sleep(1);
    delete sensor;
    
    if(lastMessage != (longString + "=0")) {
        cout << "Message wasn't transmitted correctly, test failed" << endl;
        //exit(EXIT_FAILURE);
        return false;
    }
    
    cout << "Test successful" << endl;
    return true;
}

bool httpLengthTest(sockaddr_in addr, RESTManager manager) {
    cout << endl << "Checking for response time of HttpServer with huge history" << endl;
    time_t sent, received;
    
    manager.initStructure();
    manager.put("Käse", "4");
    manager.put("Bread", "5");
    manager.put("Milk", "6");
    manager.put("Juice", "7");
    
    string history = "";
    for(int i = 0; i < 1000000; i++)
        history += "Käse=" + to_string(i) + "\n";
    
    manager.put("history", history);
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    connect(sockfd, (sockaddr*)&addr, sizeof(addr));
    
    string request = "GET / HTTP/1.1\r\n\r\n";
    char response[4096];
    sent = time(NULL);
    send(sockfd, request.c_str(), request.length(), 0);
    recv(sockfd, response, 4096, 0);
    received = time(NULL);
    
    if(string(response).find("HTTP/1.1 200 OK") == string::npos || (received - sent) >= 2) {
        cout << "Test failed, didn't receive Http response in less than 2 seconds (" << to_string(received-sent) << ")" << endl;
        //exit(EXIT_FAILURE);
        close(sockfd);
        return false;
    }
    close(sockfd);
    cout << "Received response from HttpServer in less than 2 seconds, test successful" << endl;
    return true;
}

bool httpLengthIntegrityTest(sockaddr_in addr, RESTManager manager) {
    cout << endl << "Checking for correct http-response with huge history" << endl;
    
    manager.initStructure();
    manager.put("Käse", "4");
    manager.put("Bread", "5");
    manager.put("Milk", "6");
    manager.put("Juice", "7");
    
    string history = "";
    for(int i = 0; i < 1000000; i++)
        history += "Käse=" + to_string(i) + "\n";
    
    manager.put("history", history);
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    connect(sockfd, (sockaddr*)&addr, sizeof(addr));
    
    string request = "GET / HTTP/1.1\r\n\r\n";
    string expectedResponse = generateHttpResponse(manager);
    expectedResponse.shrink_to_fit();
    string response = "";
    unsigned long bRead = 0;
    
    send(sockfd, request.c_str(), request.length(), 0);
    do {
        char segment[4096] = {'\0'};
        bRead += recv(sockfd, segment, 4096, 0);
        response += segment;
    } while(bRead < expectedResponse.length());
    
    expectedResponse.shrink_to_fit();
    if(response != expectedResponse) {
        cout << "Test failed, didn't receive correct http response " << expectedResponse.compare(response) << endl;
        cout << response.substr(expectedResponse.length()-6) << endl;
        //exit(EXIT_FAILURE);
        close(sockfd);
        return false;
    }
    close(sockfd);
    cout << "Received correct response from HttpServer, test successful" << endl;
    return true;
}

bool producerOfferTest(string brokerIP) {
    cout << endl << "Checking if Producers send offers for at least 90% of 1000 offers" << endl;
    
    bool success = false;
    TestCallback callback;
    const char* item = "Cheese";
    MQTTClient client("test", brokerIP.c_str(), &callback);
    
    if(char* channel = (char*)malloc(strlen(OFFER_PREFIX)+strlen(item)+1)) {
        
        strcpy(channel, OFFER_PREFIX);
        strcat(channel, item);
        
        client.subscribe(channel);
        client.loop();
        
        free(channel);
        
        if(channel = (char*)malloc(strlen(DEMAND_PREFIX)+strlen(item)+1)) {

            strcpy(channel, DEMAND_PREFIX);
            strcat(channel, item);
            
            for(int i = 0; i < 1000; i++) {
                MQTTClient::Offer offer;
                strncpy(offer.item, item, MAX_ITEM_LEN);
                offer.amount = 1;
                offer.price = 1000+i;

                if(client.publish(channel, &offer, sizeof(offer))) 
                        client.loop();
                else
                    i--;
                
            }
            free(channel);
            
            if(callback.getReceivedOffes() >= 900)
                success = true;
        }
    }
    
    if(!success)
        cout << "Test failed, did only receive " << callback.getReceivedOffes() << " offers" << endl;
    else
        cout << "Test successful, received " << callback.getReceivedOffes() << " offers" << endl;
    
    return success;
}

bool storeOrderTest(string brokerIP) {
    cout << endl << "Checking if Stores send orders" << endl;
    
    bool success = false;
    TestCallback callback;
    const char* item = "Cheese";
    MQTTClient client("test", brokerIP.c_str(), &callback);
    
    client.subscribe(ORDER_CHANNEL);
    client.loop();
    if(char* channel = (char*)malloc(strlen(OFFER_PREFIX)+strlen(item)+1)) {
        
        strcpy(channel, OFFER_PREFIX);
        strcat(channel, item);
        
        for(int i = 0; i < 1000; i++) {
            MQTTClient::Offer offer;
            strncpy(offer.item, item, MAX_ITEM_LEN);
            offer.amount = INITIAL_STOCK;
            offer.price = 1;
            
            if(client.publish(channel, &offer, sizeof(offer)))
                client.loop();
            else
                i--;
        }
        free(channel);
    }
    
    if(callback.getReceivedOrders() >= 900)
        success = true;
    
     if(!success)
        cout << "Test failed, did only receive " << callback.getReceivedOrders() << " orders" << endl;
    else
        cout << "Test successful, received " << callback.getReceivedOrders() << " orders" << endl;
    
    return success;
}

int main(int argc, char** argv) {
    
    
    string items[] = {STORE_ITEMS};
    int prices[STORE_COUNT][4] = {{ 220, 150, 130, 180},
                                  { 200, 155, 120, 200}};
    string resources[] = {STORE_ITEMS, "history"};
    
    RESTManager manager(resources, 5);
    manager.initStructure();
    
    StoreManager* stores[STORE_COUNT];
    string storeIps[STORE_COUNT];
    int storePorts[STORE_COUNT];
    int port = stoi(manager.getConfig("Store1Port"));
    string brokerIp = manager.getConfig("BrokerIp");
    for(int i = 0; i < STORE_COUNT; i++) {
        storeIps[i] = manager.getConfig("Store1Ip");
        storePorts[i] = port++;
        stores[i] = new StoreManager(storePorts[i], items, prices[i], sizeof(prices[i]) / sizeof(int), brokerIp, i, 0);
    }
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(manager.getConfig("ServerIp").c_str());
    addr.sin_port = htons(stoi(manager.getConfig("ServerPort")));
    
    sockaddr_in addrHttp = addr;
    addrHttp.sin_port=htons(stoi(manager.getConfig("HttpServerPort")));
    addrHttp.sin_addr.s_addr = inet_addr(manager.getConfig("HttpServerIp").c_str());
    
    Server server(addr, manager, storeIps, storePorts, STORE_COUNT);
    server.addObserver(serverReceivedMessage);
    HttpServer httpServer(addrHttp, manager);
    
    thread serverThread(&Server::receive, &server);
    thread httpServerThread(&HttpServer::start, &httpServer);
    thread* storeServerThreads[STORE_COUNT];
    
     
    const char* farmProducts[] = {"Milk", "Cheese"};
    const char* marketProducts[] = {"Bread", "Juice"};
    const char* brokerAddr = brokerIp.c_str();
    
    Producer farm1("farm1", brokerAddr, farmProducts, 2);
    Producer farm2("farm2", brokerAddr, farmProducts, 2);
    Producer market1("markt1", brokerAddr, marketProducts, 2);
    Producer market2("markt2", brokerAddr, marketProducts, 2);
    
    
    thread tFarm1(&Producer::start, &farm1);
    thread tFarm2(&Producer::start, &farm2);
    thread tMarket1(&Producer::start, &market1);
    thread tMarket2(&Producer::start, &market2);
    
    sleep(1);
    
    bool load = loadTest();
    bool length = lengthTest();
    bool httpLength = httpLengthTest(addrHttp, manager);
    bool httpIntegrity = httpLengthIntegrityTest(addrHttp, manager);
    bool offers = producerOfferTest(brokerIp);
    
    farm1.stop();
    farm2.stop();
    market1.stop();
    market2.stop();
    
    sleep(1);
    
    for(int i = 0; i < STORE_COUNT; i++) {
        storeServerThreads[i] = new thread(&StoreManager::startLoop, stores[i]);
    }
    
    bool orders = storeOrderTest(brokerIp);
    
    serverThread.detach();
    httpServerThread.detach();
    
    cout << endl << "===================================================" << endl;
    cout << endl << endl << "All stress tests were executed" << endl << endl;
    cout << "Server load stress test: " << (load ? "success" : "fail") << endl;
    cout << "Server length stress test: " << (length ? "success" : "fail") << endl;
    cout << "Http server length stress test: " << (httpLength ? "success" : "fail") << endl;
    cout << "Http server integrity stress test: " << (httpIntegrity ? "success" : "fail") << endl;
    cout << "Producer stress test: " << (offers ? "success" : "fail") << endl;
    cout << "Store stress test: " << (orders ? "success" : "fail") << endl;
    cout << endl << "===================================================" << endl;
    
    for(int i = 0; i < STORE_COUNT; i++) {
        stores[i]->stopLoop();
        storeServerThreads[i]->join();
    }
    
    return (EXIT_SUCCESS);
}

string generateTables(RESTManager manager) {
    string table = "<h3>Aktuell</h3><table><tr><th>Produkt</th><th>Menge</th></tr>";
    list<string> items = manager.get(INDEX_NAME);
    items.remove("history");
    for(string item : items) {
        if(manager.get(item).size() > 0) {
            table += "<tr><td>" + item + "</td><td>" + manager.get(item).front() + "</td></tr>";
             if (table.find("ä") != string::npos)
                table.replace(table.find("ä"), 2, "&auml;");
        }
    }
    table += "</table>";
    
    table += "<h3>History</h3><table><tr><th>Produkt</th><th>Menge</th></tr>";
    list<string> history = manager.get("history");
    for(string entry : history) {
        int pos = entry.find('=');
        string name = entry.substr(0, pos);
        string amount = entry.substr(pos+1, entry.length()-pos);
        
        unsigned long aindex = name.find("ä");
        unsigned long oindex = name.find("ö");
        unsigned long uindex = name.find("ü");
        
        if (aindex != string::npos)
                name.replace(aindex, 2, "&auml;");
        if (oindex != string::npos)
                name.replace(oindex, 2, "&ouml;");
        if (uindex != string::npos)
                name.replace(uindex, 2, "&uuml;");
        
        table += "<tr><td>" + name + "</td><td>" + amount + "</td></tr>";
    }
    table += "</table>";
    
    return table;
}

string generateHttpResponse(RESTManager manager) {
    string html = "<html><header><meta http-equiv=\"refresh\" content=\"1\" /><title>Test</title></header><body>" +
           generateTables(manager) + "</body></html>";
    string header = "HTTP/1.1 200 OK\r\nContent-Length: " + to_string(html.length()) + "\r\n" +
             "Content-Type: text/html\r\nConnection: close\r\n\r\n";

    return header + html;
}