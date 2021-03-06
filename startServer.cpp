/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   startServer.cpp
 * Author: tim
 *
 * Created on 20. April 2017, 20:42
 */

#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <string.h>

#include "StoreManager.h"
#include "RESTManager.h"
#include "Server.h"
#include "HttpServer.h"

#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 27015
#define STORE_COUNT 2
#define STORE_ITEMS "Cheese", "Bread", "Milk", "Juice"

using namespace std;

int main(int argc, char** argv) {
    
    string items[] = {STORE_ITEMS};
    int prices[STORE_COUNT][4] = {{ 220, 150, 130, 180},
                                  { 200, 155, 120, 200}};
    string resources[] = {STORE_ITEMS, "history"};
    
    RESTManager manager(resources, 5);
    manager.initStructure();
    
    int externalStoreN = stoi(manager.getConfig("ExternalStoreN"));
    
    string storeIps[STORE_COUNT+externalStoreN];
    int storePorts[STORE_COUNT+externalStoreN];
    
    //Local stores
    StoreManager* stores[STORE_COUNT];
    int port = stoi(manager.getConfig("Store1Port"));
    string brokerIp = manager.getConfig("BrokerIp");
    for(int i = 0; i < STORE_COUNT; i++) {
        storeIps[i] = manager.getConfig("Store1Ip");
        storePorts[i] = port++;
        stores[i] = new StoreManager(storePorts[i], items, prices[i], sizeof(prices[i]) / sizeof(int), brokerIp, i);
    }
    
    //Exernal stores
    string externalStores = manager.getConfig("ExternalStores");
    for(int i = STORE_COUNT; i < STORE_COUNT+externalStoreN; i++) {
        unsigned int pos = externalStores.find(',');
        string store = externalStores.substr(0, pos);
        unsigned int portDelim = store.find(':');
        
        storeIps[i] = store.substr(0, portDelim);
        storePorts[i] = stoi(store.substr(portDelim+1, store.find(',')-(portDelim+1)));
        externalStores.erase(0, pos+1);
    }
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(stoi(manager.getConfig("ServerPort")));
    
    Server* server = new Server(addr, manager, storeIps, storePorts, STORE_COUNT+externalStoreN);
    
    addr.sin_port = htons(stoi(manager.getConfig("HttpServerPort")));
    
    HttpServer* httpServer = new HttpServer(addr, manager);
    httpServer->addObserver(&Server::restock);
    
    thread* storeServerThreads[STORE_COUNT];
    
    for(int i = 0; i < STORE_COUNT; i++) {
        storeServerThreads[i] = new thread(&StoreManager::startLoop, stores[i]);
    }
    
    sleep(1);
    thread serverThread(&Server::receive, server);
    thread httpServerThread(&HttpServer::start, httpServer);
    
    
    serverThread.join();
    httpServerThread.join();
    
    for(thread* thread : storeServerThreads) {
        thread->join();
    }
    
    return 0;
}

