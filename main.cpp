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

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 27015

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    addr.sin_port = htons(SERVER_PORT);
    
    thread serverThread(&Server::receive, new Server(addr));
    
    sleep(1);
    
    thread* sensorThreads[4];
    
    sensorThreads[0] = new thread(&Sensor::send, new Sensor("Cheese", addr));
    sensorThreads[1] = new thread(&Sensor::send, new Sensor("Bread", addr));
    sensorThreads[2] = new thread(&Sensor::send, new Sensor("Milk", addr));
    sensorThreads[3] = new thread(&Sensor::send, new Sensor("Orange Juice", addr));
    
    for(int i = 0; i < 4; i++)
        sensorThreads[i]->join();
    
    sleep(1);
    
    serverThread.detach();
}

