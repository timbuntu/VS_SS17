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
#include <fstream>
#include <string>
#include <vector>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

#include "RESTManager.h"

#include "gen-cpp/Store.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;


class Server {
public:
    Server(sockaddr_in addr, RESTManager manager, std::string* storeIps, int* storePorts, unsigned int nStores);
    virtual ~Server();
    
    void receive();
    std::vector<std::string> getReceivedMessages() const { return receivedMessages; }
    bool hasReceivedMessages() const { return !receivedMessages.empty(); }
    
    void addObserver(void (*observer)(std::string));
    void notifyObservers(std::string info) const;
    
private:
    
    int sockfd;
    bool bound;
    sockaddr_in addr;
    RESTManager manager;
    std::vector<std::string> receivedMessages;
    std::vector<void (*)(std::string)> observers;
    StoreClient** clients;
    boost::shared_ptr<TTransport>** transport;
    unsigned int nStores;
    
    void saveReading(std::string message);
};

#endif /* SERVER_H */

