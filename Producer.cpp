/* 
 * File:   Producer.cpp
 * Author: debian
 * 
 * Created on 15. Juni 2017, 17:07
 */

#include "Producer.h"

Producer::Producer(const char* id, const char* addr, char** products, unsigned int n) {
    this->client = new MQTTClient(id, addr);
    this->products = products;
    this->n = n;
}

Producer::~Producer() {
    delete client;
}

void Producer::start() {
    for(int i = 0; i < n; i++) {
        char* channel = (char*)malloc(strlen(products[i])+ strlen(DEMAND_PREFIX));
        strcpy(channel, DEMAND_PREFIX);
        strcat(channel, products[i]);
        client->subscribe(channel);
        free(channel);
    }
}
