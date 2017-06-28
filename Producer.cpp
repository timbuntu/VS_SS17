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
        if(char* channel = (char*)malloc(strlen(products[i])+ strlen(DEMAND_PREFIX)+1)) {
            strcpy(channel, DEMAND_PREFIX);
            strcat(channel, products[i]);
            client->subscribe(channel);
            free(channel);
        }
    }
    
    client->subscribe(ORDER_CHANNEL);
    
    while(true) {
        client->loop();
        //printf("Producer alive\n");
    }
}

void Producer::on_mosqEvent(const char* channel, const void* msg) {
    MQTTClient::Offer* offer = (MQTTClient::Offer*)msg;
    //printf("%d times %s for %d each\n", offer->amount, offer->item, offer->price);
    
    if(strstr(channel, DEMAND_PREFIX)) {
        const char* item = channel + strlen(DEMAND_PREFIX);
        if(!strcmp(item, offer->item) && 20+rand()%21+offer->price/5 > 50) {
            
            if(char* responseChannel = (char*)malloc(strlen(item)+strlen(OFFER_PREFIX)+1)) {
                strcpy(responseChannel, OFFER_PREFIX);
                strcat(responseChannel, item);
                client->publish(responseChannel, offer, sizeof(*offer));
                //printf("Created offer for %d %s responding to demand\n", offer->amount, offer->item);
                
                strncpy(lastOffer.item, offer->item, MAX_ITEM_LEN);
                lastOffer.price = offer->price;
                lastOffer.amount = offer->amount;
                
                free(responseChannel);
            }
        }
    } else if(strstr(channel, ORDER_CHANNEL)) {
        //printf("Order for %s received\n", offer->item);
        if(!strncmp(offer->item, lastOffer.item, MAX_ITEM_LEN) && offer->price >= lastOffer.price && offer->amount <= lastOffer.amount) {
            client->publish(CONFIRM_CHANNEL, offer, sizeof(*offer));
            *lastOffer.item = 0;
            //printf("Order for %d %s accepted\n", offer->amount, offer->item);
        } else  {
            //printf("Order for %d %s ignored\n", offer->amount, offer->item);
        }
    }
}
