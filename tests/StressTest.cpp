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
#include "../Sensor.h"
#include "../Server.h"
#include "../HttpServer.h"

#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 27015

using namespace std;

sockaddr_in addr;
string lastMessage;
unsigned int receivedMessageCount = 0;

string generateHttpResponse(RESTManager manager);

void serverReceivedMessage(string message) {
    receivedMessageCount++;
    lastMessage = message;
    cout << "Message Received" << endl;
}

void loadTest() {
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
        return;
    }
    
    cout << "Test successful" << endl;
}

void lengthTest() {
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
        return;
    }
    
    cout << "Test successful" << endl;
}

void httpLengthTest(sockaddr_in addr, RESTManager manager) {
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
        return;
    }
    close(sockfd);
    cout << "Received response from HttpServer in less than 2 seconds, test successful" << endl;
}

void httpLengthIntegrityTest(sockaddr_in addr, RESTManager manager) {
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
        return;
    }
    close(sockfd);
    cout << "Received correct response from HttpServer, test successful" << endl;
}

int main(int argc, char** argv) {
    
    string resources[] = {"Käse", "Bread", "Milk", "Juice", "history"};

    RESTManager manager(resources, 5);
    manager.initStructure();
    
    addr.sin_family = AF_INET;
    
    if(argc == 3) {
        addr.sin_addr.s_addr = inet_addr(argv[1]);
        addr.sin_port = htons(stoi(argv[2]));
    }
    else {
        addr.sin_addr.s_addr = inet_addr(DEFAULT_ADDRESS);
        addr.sin_port = htons(DEFAULT_PORT);
    }
    
    sockaddr_in addrHttp = addr;
    addrHttp.sin_port=htons(15000);
    
    Server server(addr, manager);
    server.addObserver(serverReceivedMessage);
    HttpServer httpServer(addrHttp, manager);
    
    thread serverThread(&Server::receive, &server);
    thread httpServerThread(&HttpServer::start, &httpServer);
    
    sleep(1);
    
    loadTest();
    lengthTest();
    httpLengthTest(addrHttp, manager);
    httpLengthIntegrityTest(addrHttp, manager);
    
    serverThread.detach();
    httpServerThread.detach();
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