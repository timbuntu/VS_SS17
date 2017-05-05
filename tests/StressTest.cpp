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
    cout << "Message Received" << endl;
}

thread* init(string serverIp, unsigned short serverPort) {
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(serverIp.c_str());
    addr.sin_port = htons(serverPort);
    
    Server server(addr);
    server.addObserver(serverReceivedMessage);
    
    return new thread(&Server::receive, &server);
}

void loadTest() {
    cout << endl << "Testing behavior under heavy load" << endl;
    receivedMessageCount = 0;
    thread* threads[1000];
    for(int i = 0; i < 1000; i++)
        threads[i] = new thread(&Sensor::send, new Sensor(to_string(i), addr, 10, 1));
    
    for(thread* sensorThread : threads)
        sensorThread->join();
    
    sleep(1);
    
    if(receivedMessageCount < 1000*10) {
        cout << "Server overloaded, only " << receivedMessageCount << " received, test failed" << endl;
        exit(EXIT_FAILURE);
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
    
    
    cout << endl << "All tests were successful" << endl;
    
    serverThread->detach();
    return (EXIT_SUCCESS);
}

