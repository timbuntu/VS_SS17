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

#define INITIAL_STOCK 1000
#define OFFER_DELAY 10
#define PRICE_INCREASE 5

class StoreManager : public MosqCallback {
public:
    StoreManager(int port, std::string* items, int* prices, unsigned long n, std::string brokerIp, int clientID, int startStock = INITIAL_STOCK);
    virtual ~StoreManager();
    
    void startLoop();
    void stopLoop();
    
    void on_mosqEvent(const char* channel, const void* msg);
    
private:
    
    bool isActiveOrder(MQTTClient::Offer* offer) const;
    
    std::map<std::string, unsigned int> itemPrices;
    std::map<std::string, unsigned int> itemStock;
    std::map<std::string, unsigned int> demandCount;
    std::vector<Order> ordersReceived;
    MQTTClient::Offer activeOrder;
    std::thread* serverThread;
    MQTTClient client;
    bool stop;
};

#endif	/* STORE_H */

