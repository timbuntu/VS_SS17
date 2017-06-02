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
#include "../gen-cpp/Store_server.skeleton.cpp"
#include "../Server.h"
#include "../Sensor.h"
#include "../RESTManager.h"
#include "../HttpServer.h"

#define DEFAULT_ADDRESS "127.0.0.1"
#define STORE_COUNT 2
#define STORE_ITEMS "Cheese", "Bread", "Milk", "Juice"
#define DEFAULT_PORT 27015

using namespace std;

bool messageReceived = false;
timeval sent, received;
vector<string> receivedMessages;

bool wasMessageReceived(string message) {
    for(string receivedMessage : receivedMessages)
        if(receivedMessage == message)
            return true;
    
    return false;
}

void integrityTest() {
    cout << endl << "Testing integrity of received messages" << endl;
    
    if(!(wasMessageReceived("Käse=0") && wasMessageReceived("Bread=0") 
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

int main(int argc, char** argv) {
    
    string serverIpAddress;
    unsigned short serverPort;
    
    string items[] = {STORE_ITEMS};
    int prices[STORE_COUNT][4] = {{ 220, 150, 130, 180},
                                  { 200, 155, 120, 200}};
    string resources[] = {STORE_ITEMS, "history"};
    
    RESTManager manager(resources, 5);
    manager.initStructure();
    
    string storeIps[STORE_COUNT];
    int storePorts[STORE_COUNT];
    int port = stoi(manager.getConfig("Store1Port"));
    for(int i = 0; i < STORE_COUNT; i++) {
        storeIps[i] = manager.getConfig("Store1Ip");
        storePorts[i] = port++;
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
    thread* storeServerThreads[STORE_COUNT];
    
    for(int i = 0; i < STORE_COUNT; i++) {
        storeServerThreads[i] = new thread(StoreHandler::startStoreServer, storePorts[i], items, prices[i], sizeof(prices[i]) / sizeof(int));
    }
    
    sleep(1);
    
    Sensor* sensors[4];
    
    sensors[0] = new Sensor("Käse", addr, 1, 1);
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
    
    httpResponse(addr);
    
    httpServerError(addr);
    
    cout << endl << "All tests were successful" << endl;
    
    serverThread.detach();
    httpServerThread.detach();
    
    for(thread* thread : storeServerThreads) {
        thread->detach();
    }
    
    return (EXIT_SUCCESS);
}

