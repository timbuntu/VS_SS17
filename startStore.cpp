/* 
 * File:   startStore.cpp
 * Author: debian
 *
 * Created on 2. Juni 2017, 05:15
 */

#include <cstdlib>
#include <string>
#include <thread>

#include "StoreManager.h"
#include "RESTManager.h"

#define STORE_COUNT 2
#define STORE_ITEMS "Cheese", "Bread", "Milk", "Juice"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    
    string items[] = {STORE_ITEMS};
    int prices[STORE_COUNT][4] = {{ 190, 150, 115, 190},
                                  { 230, 175, 120, 210}};
    
    RESTManager manager(nullptr, 0);
    manager.initStructure();
    
    string storeIps[STORE_COUNT];
    int storePorts[STORE_COUNT];
    string ip = manager.getConfig("Store1Ip");
    string brokerIp = manager.getConfig("BrokerIp");
    int port = stoi(manager.getConfig("Store1Port"));
    
    StoreManager* stores[STORE_COUNT];
    
    for(int i = 0; i < STORE_COUNT; i++) {
        storeIps[i] = ip;
        storePorts[i] = port++;
        stores[i] = new StoreManager(storePorts[i], items, prices[i], sizeof(prices[i]) / sizeof(int), brokerIp, i);
    }
    
    thread* storeServerThreads[STORE_COUNT];
    
    for(int i = 0; i < STORE_COUNT; i++) {
        storeServerThreads[i] = new thread(&StoreManager::startLoop, stores[i]);
    }
    
    for(thread* thread : storeServerThreads) {
        thread->join();
    }
    
    return 0;
}

