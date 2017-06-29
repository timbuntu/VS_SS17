/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Test.cpp
 * Author: tim
 *
 * Created on 20. April 2017, 21:36
 */

#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../StoreManager.h"
#include "../Server.h"
#include "../Sensor.h"
#include "../RESTManager.h"
#include "../HttpServer.h"
#include "../MQTTClient.h"
#include "../Producer.h"

#define DEFAULT_ADDRESS "127.0.0.1"
#define STORE_COUNT 2
#define STORE_ITEMS "Cheese", "Bread", "Milk", "Juice"
#define DEFAULT_PORT 27015

using namespace std;

bool messageReceived = false;
timeval sent, received;
vector<string> receivedMessages;

class TestCallback : public MosqCallback {
public:

    void on_mosqEvent(const char* channel, const void* msg) {
        cout << "Callback called" << endl;
        
        if(!strcmp(channel, ORDER_CHANNEL))
            order = true;
        else if(strstr(channel, OFFER_PREFIX))
            offer = true;
    }
    
    bool receivedOrder() const { return order; }
    bool receivedOffer() const { return offer; }
    
private:
    bool order = false;
    bool offer = false;

};

bool wasMessageReceived(string message) {
    for(string receivedMessage : receivedMessages)
        if(receivedMessage == message)
            return true;
    
    return false;
}

void integrityTest() {
    cout << endl << "Testing integrity of received messages" << endl;
    
    if(!(wasMessageReceived("Cheese=0") && wasMessageReceived("Bread=0") 
                                          && wasMessageReceived("Milk=0") && wasMessageReceived("Orange Juice=0"))) {
        
        cout << "Test of integrity failed" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Test for integrity was successful" << endl;
}

void timeTest() {
    cout << endl << "Testing processing time for messages" << endl;
    unsigned long delta = (received.tv_sec*1000000+received.tv_usec) - (sent.tv_sec*1000000+sent.tv_usec);
    cout << "Processing of message took " << delta << " microseconds" << endl;
    if(delta > 100000) {
        cout << "Processing time was over 100 milliseconds, test failed." << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Test for processing time was successful" << endl;
}

void sensorTest(Sensor* sensor) {
    cout << endl << "Testing "  << sensor->getItem() << "-sensor for sent messages" << endl;
    if(sensor->getSentMessages().empty()) {
        cout << "Sensor did not send any messages" << endl;
        cout << "Test for sensor failed" << endl;
        exit(EXIT_FAILURE);
    }
    
    cout << "Test for sensor was successful" << endl;
}

void serverTest(Server server) {
    cout << endl << "Testing server for received messages" << endl;
    if(!messageReceived) {
        cout << "Server did not receive any messages" << endl;
        cout << "Test for server failed" << endl;
        exit(EXIT_FAILURE);
    }
    
    cout << "Test for server was successful" << endl;
}

void serverReceivedMessage(string message) {
    if(!messageReceived) {
        messageReceived = true;
        gettimeofday(&received, NULL);
    }
    receivedMessages.push_back(message);
}

void httpResponse(sockaddr_in addr) {
    cout << endl << "Checking for HttpServer response" << endl;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    connect(sockfd, (sockaddr*)&addr, sizeof(addr));
    
    string request = "GET / HTTP/1.1\r\n\r\n";
    char response[4096];
    
    send(sockfd, request.c_str(), request.length(), 0);
    recv(sockfd, response, 4096, 0);
    if(string(response).find("HTTP/1.1") == string::npos) {
        cout << "Test failed, didn't receive Http response" << endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    close(sockfd);
    cout << "Received response from HttpServer, test successful" << endl;
}

void httpServerError(sockaddr_in addr) {
    cout << endl << "Checking HttpServer response in case of invalid URI" << endl;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    connect(sockfd, (sockaddr*)&addr, sizeof(addr));
    
    string request = "GET /wasd HTTP/1.1\r\n\r\n";
    char response[4096];
    
    send(sockfd, request.c_str(), request.length(), 0);
    recv(sockfd, response, 4096, 0);
    if(string(response).find("404 Not Found") == string::npos) {
        cout << "Test failed, didn't receive error from server" << endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    close(sockfd);
    cout << "Received error from HttpServer, test successful" << endl;
}

void producerOfferTest(string brokerIP) {
    cout << endl << "Checking if Producer send offers" << endl;
    
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

            MQTTClient::Offer offer;
            strncpy(offer.item, item, MAX_ITEM_LEN);
            offer.amount = 10;
            offer.price = 1000;
            
            if(client.publish(channel, &offer, sizeof(offer))) {
                cout << "Published to " << channel << endl;
                for(int i = 0; i < 10; i++) {
                    client.loop();
                    if(callback.receivedOffer()) {
                        success = true;
                        break;
                    }
                }
            }
            free(channel);
        }
    }
    
    if(!success) {
        cout << "Test failed, didn't receive offer from producer" << endl;
        exit(EXIT_FAILURE);
    } else
        cout << "Test successful, received offer from producer" << endl;
}

void storeOrderTest(string brokerIP) {
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
        
        MQTTClient::Offer offer;
        strncpy(offer.item, item, MAX_ITEM_LEN);
        offer.amount = INITIAL_STOCK;
        offer.price = 1;
        if(client.publish(channel, &offer, sizeof(offer))) {
            cout << "Published to " << channel << endl;
            for(int i = 0; i < 10; i++) {
                client.loop();
                if(callback.receivedOrder()) {
                    success = true;
                    break;
                }
            }
        }
        free(channel);
    }
    
    if(!success) {
        cout << "Test failed, didn't receive order from store" << endl;
        exit(EXIT_FAILURE);
    } else
        cout << "Test successful, received order from store" << endl;
}

int main(int argc, char** argv) {
    
    string serverIpAddress;
    unsigned short serverPort;
    
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
        stores[i] = new StoreManager(storePorts[i], items, prices[i], sizeof(prices[i]) / sizeof(int), brokerIp, i, INITIAL_STOCK/2);
    }
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(manager.getConfig("ServerIp").c_str());
    addr.sin_port = htons(stoi(manager.getConfig("ServerPort")));
    
    sockaddr_in addrHttpServer = addr;
    addrHttpServer.sin_port= htons(stoi(manager.getConfig("HttpServerPort")));
    addrHttpServer.sin_addr.s_addr = inet_addr(manager.getConfig("HttpServerIp").c_str());
    
    Server server(addr, manager, storeIps, storePorts, STORE_COUNT);
    server.addObserver(serverReceivedMessage);
    HttpServer httpServer(addrHttpServer, manager);
    
    thread serverThread(&Server::receive, &server);
    thread httpServerThread(&HttpServer::start, &httpServer);
    
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
    
    producerOfferTest(brokerIp);
    
    farm1.stop();
    farm2.stop();
    market1.stop();
    market2.stop();
    
    thread* storeServerThreads[STORE_COUNT];
    
    for(int i = 0; i < STORE_COUNT; i++) {
        storeServerThreads[i] = new thread(&StoreManager::startLoop, stores[i]);
    }
    
    sleep(1);
    
    Sensor* sensors[4];
    
    sensors[0] = new Sensor("Cheese", addr, 1, 1);
    sensors[1] = new Sensor("Bread", addr, 1, 1);
    sensors[2] = new Sensor("Milk", addr, 1, 1);
    sensors[3] = new Sensor("Orange Juice", addr, 1, 1);
    
    thread* sensorThreads[4];
    
    gettimeofday(&sent, NULL);
    sensorThreads[0] = new thread(&Sensor::send, sensors[0]);
    sensorThreads[1] = new thread(&Sensor::send, sensors[1]);
    sensorThreads[2] = new thread(&Sensor::send, sensors[2]);
    sensorThreads[3] = new thread(&Sensor::send, sensors[3]);
    
    for(int i = 0; i < 4; i++) 
        sensorThreads[i]->join();
    
    sleep(1);
    
    for(int i = 0; i < 4; i++)
        sensorTest(sensors[i]);
    
    serverTest(server);
    
    timeTest();
    
    integrityTest();
    
    httpResponse(addrHttpServer);
    
    httpServerError(addrHttpServer);
    
    storeOrderTest(brokerIp);
    
    cout << endl << "===================================================" << endl;
    cout << endl << endl << "All tests were successful" << endl << endl;
    cout << endl << "===================================================" << endl;
    
    
    for(int i = 0; i < STORE_COUNT; i++) {
        stores[i]->stopLoop();
        storeServerThreads[i]->join();
    }
    
    serverThread.detach();
    httpServerThread.detach();
    
    tFarm1.detach();
    tFarm2.detach();
    tMarket1.detach();
    tMarket2.detach();
    
    return EXIT_SUCCESS;
}

