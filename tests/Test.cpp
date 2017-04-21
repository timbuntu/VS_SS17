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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../Server.h"
#include "../Sensor.h"

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 27015

using namespace std;

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
    if(server.getReceivedMessages().empty()) {
        cout << "Server did not receive any messages" << endl;
        cout << "Test for server failed" << endl;
        exit(EXIT_FAILURE);
    }
    
    cout << "Test for sensor was successful" << endl;
}

int main(int argc, char** argv) {
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    addr.sin_port = htons(SERVER_PORT);
    
    Server server(addr);
    
    thread serverThread(&Server::receive, &server);
    
    sleep(1);
    
    Sensor* sensors[4];
    
    sensors[0] = new Sensor("Cheese", addr);
    sensors[1] = new Sensor("Bread", addr);
    sensors[2] = new Sensor("Milk", addr);
    sensors[3] = new Sensor("Orange Juice", addr);
    
    thread* sensorThreads[4];
    
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
    
    serverThread.detach();

    return (EXIT_SUCCESS);
}

