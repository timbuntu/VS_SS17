/* 
 * File:   Store.h
 * Author: debian
 *
 * Created on 21. Juni 2017, 08:56
 */

#ifndef STORE_H
#define	STORE_H

#include <string>
#include <thread>
#include <map>
#include <vector>

#include "gen-cpp/Store_server.skeleton.cpp"
#include "MQTTClient.h"

class StoreManager : public MosqCallback {
public:
    StoreManager(int port, std::string* items, int* prices, unsigned long n, std::string brokerIp, int clientID);
    virtual ~StoreManager();
    
    void startLoop();
    
    void on_mosqEvent(const void* msg);
    
private:
    std::map<std::string, unsigned int> itemPrices;
    std::vector<Order> orders;
    std::thread* serverThread;
    MQTTClient client;
};

#endif	/* STORE_H */

