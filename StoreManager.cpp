/* 
 * File:   Store.cpp
 * Author: debian
 * 
 * Created on 21. Juni 2017, 08:56
 */

#include "StoreManager.h"

StoreManager::StoreManager(int port, std::string* items, int* prices, unsigned long n, std::string brokerIp, int clientID) 
: client(std::string("store" + std::to_string(clientID)).c_str(), brokerIp.c_str(), this)
{
    
    for (int i = 0; i < n; i++) {
        itemPrices.insert(std::pair<std::string, int>(items[i], prices[i]));
        client.subscribe(std::string("offer/" + items[i]).c_str());
    }
    
    serverThread = new std::thread(StoreHandler::startStoreServer, port, &itemPrices, &orders);
}

StoreManager::~StoreManager() {
}

void StoreManager::startLoop() {
    
    
    serverThread->join();
}

void StoreManager::on_mosqEvent(const void* msg) {
    MQTTClient::Offer* offer = (MQTTClient::Offer*)msg;
    printf("Offer: %d times %s for %d each\n", offer->amount, offer->item, offer->price);
}
