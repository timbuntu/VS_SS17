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

#include "gen-cpp/Store_server.skeleton.cpp"
#include "Server.h"
#include "Sensor.h"
#include "HttpServer.h"

#define DEFAULT_PORT 27015
#define STORE_ITEMS "Käse", "Bread", "Milk", "Juice"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    string items[] = {STORE_ITEMS};
    unsigned int prices[] = { 220, 150, 130, 180};
    string resources[] = {STORE_ITEMS, "history"};
    
    RESTManager manager(resources, 5);
    manager.initStructure();
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(stoi(manager.getConfig("HttpServerPort")));
    
    HttpServer* httpServer = new HttpServer(addr, manager);
    
    addr.sin_port = htons(stoi(manager.getConfig("ServerPort")));
    
    Server* server = new Server(addr, manager);
    
    thread serverThread(&Server::receive, server);
    thread httpServerThread(&HttpServer::start, httpServer);
    
    sleep(1);
    
    addr.sin_addr.s_addr = inet_addr(manager.getConfig("ServerIp").c_str());
    
    Sensor* sensors[] = {new Sensor("Käse", addr), new Sensor("Bread", addr), new Sensor("Milk", addr), new Sensor("Juice", addr)};
    thread* sensorThreads[4];
    
    for(int i = 0; i < 4; i++)
        sensorThreads[i] = new thread(&Sensor::send, sensors[i]);
    
    StoreHandler::startStoreServer(stoi(manager.getConfig("Store1Port")), items, prices, sizeof(prices) / sizeof(int));
    
    for(int i = 0; i < 4; i++)
        sensorThreads[i]->join();
    
    serverThread.join();
    httpServerThread.join();
    
    delete server;
    for(Sensor* sensor : sensors)
        delete sensor;
}

