/* 
 * File:   Producer.cpp
 * Author: debian
 * 
 * Created on 15. Juni 2017, 17:07
 */

#include "Producer.h"

Producer::Producer(const char* id, const char* const addr, const char** products, unsigned int n) {
    this->client = new MQTTClient(id, addr, this);
    this->products = products;
    this->n = n;
}

Producer::~Producer() {
    delete client;
}

void Producer::start() {
    for(int i = 0; i < n; i++) {
        char* channel = (char*)malloc((strlen(products[i])+ strlen(DEMAND_PREFIX)+1)*sizeof(char));
        strcpy(channel, DEMAND_PREFIX);
        strcat(channel, products[i]);
        client->subscribe(channel);
        //MQTTClient::Offer offer;
        //strncpy(offer.item, products[i], MAX_ITEM_LEN);
        //offer.price = 100;
        //offer.amount = 50;
        //client->publish(channel, &offer, sizeof(MQTTClient::Offer));
        free(channel);
    }
    while(true) {
        client->loop();
        sleep(1);
    }
}

void Producer::on_mosqEvent(const void* msg) {
    MQTTClient::Offer* offer = (MQTTClient::Offer*)msg;
    printf("%d times %s for %d each\n", offer->amount, offer->item, offer->price);
}
