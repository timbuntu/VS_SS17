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
#include <string>
#include <iostream>

class HttpServer {
public:
    HttpServer(sockaddr_in addr);
    virtual ~HttpServer();
    
    void start();
private:
    int sockfd;
    bool bound;
    
    void connectionHandler(int sockfd, sockaddr_in* clientAddr);
    std::string createTable() const;
};

#endif /* HTTPSERVER_H */

