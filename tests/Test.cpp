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
#include "../Server.h"
#include "../Sensor.h"

#define DEFAULT_ADDRESS "127.0.0.1"
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
    std::cout << endl << "Testing server for received messages" << std::endl;
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

int main(int argc, char** argv) {
    
    string serverIpAddress;
    unsigned short serverPort;
    
    if(argc == 3) {
        serverIpAddress = argv[1];
        serverPort = stoi(argv[2]);
    } else {
        serverIpAddress = DEFAULT_ADDRESS;
        serverPort = DEFAULT_PORT;
    }
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(serverIpAddress.c_str());
    addr.sin_port = htons(serverPort);
    
    Server server(addr);
    server.addObserver(serverReceivedMessage);
    
    thread serverThread(&Server::receive, &server);
    
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
    
    cout << endl << "All tests were successful" << endl;
    
    serverThread.detach();
    return (EXIT_SUCCESS);
}

