/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Server.h
 * Author: tim
 *
 * Created on 20. April 2017, 10:29
 */

#ifndef SERVER_H
#define SERVER_H

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <vector>

class Server {
public:
    Server(sockaddr_in addr);
    virtual ~Server();
    
    void receive();
    std::vector<std::string> getReceivedMessages() const { return receivedMessages; }
    bool hasReceivedMessages() { return !receivedMessages.empty(); }
    
    void addObserver(void (*observer)(std::string));
    void notifyObservers(std::string info);
    
private:
    
    int sockfd;
    bool bound;
    std::vector<std::string> receivedMessages;
    std::vector<void (*)(std::string)> observers;
};

#endif /* SERVER_H */

