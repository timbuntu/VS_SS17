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
#include "../Sensor.h"
#include "../Server.h"

#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 27015

using namespace std;

sockaddr_in addr;
unsigned int receivedMessageCount = 0;

void serverReceivedMessage(string message) {
    receivedMessageCount++;
}

thread* init(string serverIp, unsigned short serverPort) {
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(serverIp.c_str());
    addr.sin_port = htons(serverPort);
    
    Server server(addr);
    server.addObserver(serverReceivedMessage);
    
    return new thread(&Server::receive, &server);
}

int main(int argc, char** argv) {
    
    thread* serverThread;
    
    if(argc == 3)
        serverThread = init(argv[1], stoi(argv[2]));
    else
        serverThread = init(DEFAULT_ADDRESS, DEFAULT_PORT);
    
    
    
    sleep(1);
    
    Sensor* sensors[4];
    
    sensors[0] = new Sensor("Käse", addr, 1, 1);
    sensors[1] = new Sensor("Bread", addr, 1, 1);
    sensors[2] = new Sensor("Milk", addr, 1, 1);
    sensors[3] = new Sensor("Orange Juice", addr, 1, 1);
    
    thread* sensorThreads[4];
    sensorThreads[0] = new thread(&Sensor::send, sensors[0]);
    sensorThreads[1] = new thread(&Sensor::send, sensors[1]);
    sensorThreads[2] = new thread(&Sensor::send, sensors[2]);
    sensorThreads[3] = new thread(&Sensor::send, sensors[3]);
    
    for(int i = 0; i < 4; i++) 
        sensorThreads[i]->join();
    
    sleep(1);
    
    cout << endl << "All tests were successful" << endl;
    
    serverThread->detach();
    return (EXIT_SUCCESS);
}

