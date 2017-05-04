/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   startSensors.cpp
 * Author: tim
 *
 * Created on 20. April 2017, 20:48
 */

#include <cstdlib>
#include <thread>
#include <netinet/in.h>
#include <arpa/inet.h>

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
    
    thread* sensorThreads[4];
    
    sensorThreads[0] = new thread(&Sensor::send, new Sensor("Cheese", addr));
    sensorThreads[1] = new thread(&Sensor::send, new Sensor("Bread", addr));
    sensorThreads[2] = new thread(&Sensor::send, new Sensor("Milk", addr));
    sensorThreads[3] = new thread(&Sensor::send, new Sensor("Orange Juice", addr));
    
    for(int i = 0; i < 4; i++)
        sensorThreads[i]->join();
    
    return 0;
}

