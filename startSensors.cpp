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
#include "RESTManager.h"

#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 27015

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    RESTManager manager(nullptr, 0);
    manager.initStructure();
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(manager.getConfig("ServerIp").c_str());
    addr.sin_port = htons(stoi(manager.getConfig("ServerPort")));
    
    Sensor* sensors[] = {new Sensor("Käse", addr), new Sensor("Bread", addr), new Sensor("Milk", addr), new Sensor("Juice", addr)};
    thread* sensorThreads[4];
    
    for(int i = 0; i < 4; i++)
        sensorThreads[i] = new thread(&Sensor::send, sensors[i]);
    
    for(int i = 0; i < 4; i++)
        sensorThreads[i]->join();
    
    for(Sensor* sensor : sensors)
        delete sensor;
    
    return 0;
}

