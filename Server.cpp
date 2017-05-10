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

#include "Server.h"

Server::Server(sockaddr_in addr) {
    char addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, addr_str, INET_ADDRSTRLEN);
    std::cout << "Server listening on " << addr_str << ":" << std::to_string(ntohs(addr.sin_port)) << std::endl;
    receivedMessages.clear();
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int optionValue = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(int));
    if(sockfd >= 0) {
        bound = (bind(sockfd, (sockaddr*)&addr, sizeof(sockaddr_in)) == 0);
    }
}

Server::~Server() {
    close(sockfd);
}

void Server::receive() {
    while(bound) {
        char* buffer = new char[4096];
        int n = recv(sockfd, buffer, 4096, 0);
        std::string message(buffer, n);
        receivedMessages.push_back(message);
        std::cout << "Received: " << message << std::endl;
        notifyObservers(message);
    }
}

void Server::addObserver(void (*observer)(std::string)) {
    observers.push_back(observer);
}

void Server::notifyObservers(std::string info) {
    for(void (*observer)(std::string) : observers)
        observer(info);
}
