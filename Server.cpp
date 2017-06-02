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

RESTManager Server::manager;
std::map<std::string, int> Server::offsets;

Server::Server(sockaddr_in addr, RESTManager manager, std::string* storeIps, int* storePorts, unsigned int nStores) {
    
    this->addr = addr;
    this->manager = manager;
    this->nStores = nStores;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int optionValue = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(int));
    if(sockfd >= 0) {
        int optionValue = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(int));
        bound = (bind(sockfd, (sockaddr*)&addr, sizeof(sockaddr_in)) == 0);
        
        transport = new boost::shared_ptr<TTransport>*[nStores];
        clients = new StoreClient*[nStores];
        
        for(int i = 0; i < nStores; i++) {
            boost::shared_ptr<TSocket> socket(new TSocket(storeIps[i], storePorts[i]));
            transport[i] = new boost::shared_ptr<TTransport> (new TBufferedTransport(socket));
            boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(*transport[i]));
            clients[i] = new StoreClient(protocol);
        }
    }
    
}

Server::~Server() {
    close(sockfd);
    for(int i = 0; i < nStores; i++) {
        delete transport[i];
        delete clients[i];
    }
    delete[] transport;
    delete[] clients;
}

void Server::receive() {
    if(bound) {
        char addr_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr.sin_addr, addr_str, INET_ADDRSTRLEN);
        std::cout << "Server listening on " << addr_str << ":" << std::to_string(ntohs(addr.sin_port)) << std::endl;
    } else {
        std::cout << "Server socket not bound" << std::endl;
    }
    
    while(bound) {
        char* buffer = new char[4096];
        int n = recv(sockfd, buffer, 4096, 0);
        std::string message(buffer, n);
        //std::cout << "Received: " << message << std::endl;
        saveReading(message);
        notifyObservers(message);
        
        unsigned int pos = message.find('=');
        std::string item = message.substr(0, pos);
        int amount = (std::stoi(message.substr(pos+1))+offsets.at(item)) % 101;
        if(amount < 20) {
            int prices[nStores];
            int cheapest = 0;
            for(int i = 0; i < nStores; i++) {
                transport[i]->get()->open();
                prices[i] = clients[i]->getPrice(item);
                transport[i]->get()->close();
                //std::cout << item << " at Store" << i << ": " << prices[i] << " cents" << std::endl;
                if(prices[i] < prices[cheapest])
                    cheapest = i;
            }
            transport[cheapest]->get()->open();
            bool success = clients[cheapest]->order(item, 100-amount);
            transport[cheapest]->get()->close();
            if(success) {
                restock(item);
                std::cout << "Bought " << 100-amount << " units " << item << " for " << prices[cheapest] << " cents at Store " << cheapest << std::endl;
            }
        }
    }
}

void Server::addObserver(void (*observer)(std::string)) {
    observers.push_back(observer);
}

void Server::notifyObservers(std::string info) const {
    for(void (*observer)(std::string) : observers)
        observer(info);
}

void Server::saveReading(std::string message) {
    receivedMessages.push_back(message);
    int pos = message.find('=');
    std::string item = message.substr(0, pos);
    
    if(offsets.find(item) == offsets.end())
        offsets.insert(std::pair<std::string, int>(item, 0));
    
    int value = (stoi(message.substr(pos+1, message.length()-pos))+offsets.at(item)) % 101;
    
    manager.put("history", item + "=" + to_string(value), true);
    manager.put(item, to_string(value));
}

void Server::restock(std::string item) {
    std::cout << "Trying to restock " << item << std::endl;
    
    if(offsets.find(item) != offsets.end()) {
        int offset = offsets.at(item);
        offsets.erase(item);
        offsets.insert(std::pair<std::string, int>(item, 100-(stoi(manager.get(item).front())-offset)%101));
        manager.put("history", item + "=" + to_string(100), true);
        manager.put(item, to_string(100));
    }
}
