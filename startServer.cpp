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

#include "gen-cpp/Store_server.skeleton.cpp"
#include "RESTManager.h"
#include "Server.h"
#include "HttpServer.h"

#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 27015
#define STORE_ITEMS "Käse", "Bread", "Milk", "Juice"

using namespace std;

int main(int argc, char** argv) {
    
    string items[] = {STORE_ITEMS};
    unsigned int prices[] = { 220, 150, 130, 180};
    string resources[] = {STORE_ITEMS, "history"};
    
    RESTManager manager(resources, 5);
    manager.initStructure();
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(stoi(manager.getConfig("ServerPort")));
    
    Server* server = new Server(addr, manager);
    
    addr.sin_port = htons(stoi(manager.getConfig("HttpServerPort")));
    
    HttpServer* httpServer = new HttpServer(addr, manager);
    
    sleep(1);
    thread serverThread(&Server::receive, server);
    thread httpServerThread(&HttpServer::start, httpServer);
    
    StoreHandler::startStoreServer(stoi(manager.getConfig("Store1Port")), items, prices, sizeof(prices) / sizeof(int));
    
    serverThread.join();
    httpServerThread.join();
    
    return 0;
}

