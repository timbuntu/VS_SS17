/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Sensor.cpp
 * Author: tim
 * 
 * Created on 20. April 2017, 10:28
 */

#include <arpa/inet.h>

#include "Sensor.h"

Sensor::Sensor(std::string item, sockaddr_in addr) {
    this->item = item;
    serverAddr = addr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    reading = rand() % 100;
    steps = -3;
}

Sensor::Sensor(std::string item, sockaddr_in addr, unsigned int startingValue, int steps) {
    this->item = item;
    serverAddr = addr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    reading = startingValue;
    this->steps = steps;
}

Sensor::Sensor(const Sensor& orig) {
}

Sensor::~Sensor() {
}

void Sensor::send() {
    if(sockfd >= 0) {
        
        char addr_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &serverAddr.sin_addr, addr_str, INET_ADDRSTRLEN);
        std::cout << "Sensor sending to " << addr_str << ":" << std::to_string(ntohs(serverAddr.sin_port)) << std::endl;
        while(reading > 0) {
            
            if(reading >= -steps)
                reading += steps;
            else
                reading = 0;
            
            std::string message = item + "=" + std::to_string(reading);
            sendto(sockfd, message.c_str(), message.length(), 0, (sockaddr*)&serverAddr, sizeof(sockaddr_in));
            sentMessages.push_back(message);
            //std::cout << "Sent: " << message << std::endl;
        }
    }
}