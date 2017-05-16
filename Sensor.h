/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Sensor.h
 * Author: tim
 *
 * Created on 20. April 2017, 10:28
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <thread>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Sensor {
public:
    Sensor(std::string, sockaddr_in addr);
    Sensor(std::string, sockaddr_in addr, unsigned int startingValue, int steps);
    virtual ~Sensor();
    
    void send();
    
    std::string getItem() const { return item; }
    std::vector<std::string> getSentMessages() const { return sentMessages; }
    
    struct ItemInfo {
        char* name;
        unsigned int reading;
    };
    
private:
    
    sockaddr_in serverAddr;
    int sockfd;
    std::string item;
    unsigned int reading;
    int steps;
    std::vector<std::string> sentMessages;
    const bool manual;

};

#endif /* SENSOR_H */

