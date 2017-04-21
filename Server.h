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

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <vector>

class Server {
public:
    Server(sockaddr_in addr);
    Server(const Server& orig);
    virtual ~Server();
    
    void receive();
    std::vector<std::string> getReceivedMessages() { return receivedMessages; }
    
private:
    
    int sockfd;
    bool bound;
    std::vector<std::string> receivedMessages;
};

#endif /* SERVER_H */

