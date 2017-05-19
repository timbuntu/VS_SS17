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
string lastMessage;
unsigned int receivedMessageCount = 0;
Server* server = nullptr;

void serverReceivedMessage(string message) {
    receivedMessageCount++;
    lastMessage = message;
    cout << "Message Received" << endl;
}

thread* init(string serverIp, unsigned short serverPort) {

    string resources[] = {"Käse", "Bread", "Milk", "Juice", "history"};

    RESTManager manager(resources, 5);
    manager.initStructure();

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(serverIp.c_str());
    addr.sin_port = htons(serverPort);
    
    server = new Server(addr, manager);
    server->addObserver(serverReceivedMessage);
    
    return new thread(&Server::receive, server);
}

void loadTest() {
    cout << endl << "Testing behavior under heavy load" << endl;
    receivedMessageCount = 0;
    thread* threads[1000];
    for(int i = 0; i < 1000; i++)
        threads[i] = new thread(&Sensor::send, new Sensor(to_string(i), addr, 10, -1));
    
    for(thread* sensorThread : threads)
        sensorThread->join();
    
    sleep(5);
    
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
    
    thread sensorThread(&Sensor::send, new Sensor(longString, addr, 1, -1));
    sensorThread.join();
    sleep(1);
    
    if(lastMessage != (longString + "=0")) {
        cout << "Message wasn't transmitted correctly, test failed" << endl;
        //exit(EXIT_FAILURE);
        return;
    }
    
    cout << "Test successful" << endl;
}

int main(int argc, char** argv) {
    
    thread* serverThread;
    
    if(argc == 3)
        serverThread = init(argv[1], stoi(argv[2]));
    else
        serverThread = init(DEFAULT_ADDRESS, DEFAULT_PORT);
    
    sleep(1);
    
    loadTest();
    lengthTest();
    
    
    serverThread->detach();
    return (EXIT_SUCCESS);
}

