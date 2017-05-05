/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: tim
 *
 * Created on 20. April 2017, 10:27
 */

#include <cstdlib>
#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Server.h"
#include "Sensor.h"

#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 27015

using namespace std;

/*
 * 
 */
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
    
    thread serverThread(&Server::receive, new Server(addr));
    
    sleep(1);
    
    thread* sensorThreads[4];
    
    sensorThreads[0] = new thread(&Sensor::send, new Sensor("Käse", addr));
    sensorThreads[1] = new thread(&Sensor::send, new Sensor("Bread", addr));
    sensorThreads[2] = new thread(&Sensor::send, new Sensor("Milk", addr));
    sensorThreads[3] = new thread(&Sensor::send, new Sensor("Orange Juice", addr));
    
    for(int i = 0; i < 4; i++)
        sensorThreads[i]->join();
    
    sleep(1);
    
    serverThread.detach();
}

