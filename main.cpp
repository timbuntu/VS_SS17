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
#define STORE_COUNT 2
#define STORE_ITEMS "Cheese", "Bread", "Milk", "Juice"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    string items[] = {STORE_ITEMS};
    int prices[STORE_COUNT][4] = {{ 220, 150, 130, 180},
                                  { 200, 155, 120, 200}};
    string resources[] = {STORE_ITEMS, "history"};
    
    RESTManager manager(resources, 5);
    manager.initStructure();
    
    string storeIps[STORE_COUNT];
    int storePorts[STORE_COUNT];
    int port = stoi(manager.getConfig("Store1Port"));
    for(int i = 0; i < STORE_COUNT; i++) {
        storeIps[i] = manager.getConfig("Store1Ip");
        storePorts[i] = port++;
    }
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(stoi(manager.getConfig("HttpServerPort")));
    
    HttpServer* httpServer = new HttpServer(addr, manager);
    
    addr.sin_port = htons(stoi(manager.getConfig("ServerPort")));
    
    Server* server = new Server(addr, manager, storeIps, storePorts, STORE_COUNT);
    httpServer->addObserver(&Server::restock);
    thread* storeServerThreads[STORE_COUNT];
    
    for(int i = 0; i < STORE_COUNT; i++) {
        storeServerThreads[i] = new thread(StoreHandler::startStoreServer, storePorts[i], items, prices[i], sizeof(prices[i]) / sizeof(int));
    }
    
    sleep(1);
    thread serverThread(&Server::receive, server);
    thread httpServerThread(&HttpServer::start, httpServer);
    
    sleep(1);
    
    addr.sin_addr.s_addr = inet_addr(manager.getConfig("ServerIp").c_str());
    
    Sensor* sensors[] = {new Sensor("Cheese", addr), new Sensor("Bread", addr), new Sensor("Milk", addr), new Sensor("Juice", addr)};
    thread* sensorThreads[4];
    
    for(int i = 0; i < 4; i++)
        sensorThreads[i] = new thread(&Sensor::send, sensors[i]);
    
    for(int i = 0; i < 4; i++)
        sensorThreads[i]->join();
    
    serverThread.join();
    httpServerThread.join();
    
    for(thread* thread : storeServerThreads) {
        thread->join();
    }
    
    delete server;
    for(Sensor* sensor : sensors)
        delete sensor;
    
    return 0;
}

