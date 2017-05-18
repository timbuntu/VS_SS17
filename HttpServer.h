/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HttpServer.h
 * Author: tim
 *
 * Created on 16. Mai 2017, 12:29
 */

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <list>

#include "RESTManager.h"

class HttpServer {
public:
    HttpServer(sockaddr_in, RESTManager);
    virtual ~HttpServer();
    
    void start();
private:
    int sockfd;
    bool bound;
    sockaddr_in addr;
    RESTManager manager;
    
    void connectionHandler(int sockfd, sockaddr_in* clientAddr);
    std::string createTables() const;
};

#endif /* HTTPSERVER_H */

