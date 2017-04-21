/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Server.cpp
 * Author: tim
 * 
 * Created on 20. April 2017, 10:29
 */

#include <unistd.h>

#include "Server.h"

Server::Server(sockaddr_in addr) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd >= 0) {
        bound = (bind(sockfd, (sockaddr*)&addr, sizeof(sockaddr_in)) == 0);
    }
}

Server::Server(const Server& orig) {
    
}

Server::~Server() {
}

void Server::receive() {
    while(bound) {
        char* buffer = new char[4096];
        int n = recv(sockfd, buffer, 4096, 0);
        std::string message(buffer, n);
        receivedMessages.push_back(message);
        std::cout << message << std::endl;
        //std::cout << "Message received" << std::endl;
    }
}