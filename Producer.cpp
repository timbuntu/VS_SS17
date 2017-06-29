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
    this->productCount = n;
    this->loopCount = 0;
    this->stopProducer = false;
}

Producer::~Producer() {
    delete client;
}

void Producer::start() {
    for(int i = 0; i < productCount; i++) {
        if(char* channel = (char*)malloc(strlen(products[i])+ strlen(DEMAND_PREFIX)+1)) {
            strcpy(channel, DEMAND_PREFIX);
            strcat(channel, products[i]);
            client->subscribe(channel);
            free(channel);
        }
    }
    
    client->subscribe(ORDER_CHANNEL);
    
    while(!stopProducer) {
        client->loop();
        if(!(loopCount % OFFER_DELAY) && rand() < RAND_MAX/2)  {
            
            const char* item = products[rand()%productCount];
            if(char* channel = (char*)malloc(strlen(OFFER_PREFIX)+strlen(item)+1)) {
                
                strcpy(channel, OFFER_PREFIX);
                strcat(channel, item);
                
                MQTTClient::Offer offer;
                strncpy(offer.item, item, MAX_ITEM_LEN);
                offer.price = 60 + rand()%91;
                offer.amount = 50 + rand()%151;
                
                client->publish(channel, &offer, sizeof(offer));
                
                saveOffer(&offer);
                
                free(channel);
                //printf("Generated offer for %d %s at %d each\n", offer.amount, offer.item, offer.price);
            }
        }
        loopCount++;
        //printf("Producer alive\n");
    }
    
    client->disconnect();
    
}

void Producer::stop() {
    stopProducer = true;
}


void Producer::on_mosqEvent(const char* channel, const void* msg) {
    MQTTClient::Offer* offer = (MQTTClient::Offer*)msg;
    //printf("%d times %s for %d each\n", offer->amount, offer->item, offer->price);
    
    if(strstr(channel, DEMAND_PREFIX)) {
        const char* item = channel + strlen(DEMAND_PREFIX);
        if(!strcmp(item, offer->item) && 10+rand()%21+offer->price/5 > 50) {
            
            if(char* responseChannel = (char*)malloc(strlen(item)+strlen(OFFER_PREFIX)+1)) {
                strcpy(responseChannel, OFFER_PREFIX);
                strcat(responseChannel, item);
                client->publish(responseChannel, offer, sizeof(*offer));
                //printf("Created offer for %d %s responding to demand\n", offer->amount, offer->item);
                
                saveOffer(offer);
                
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

void Producer::saveOffer(MQTTClient::Offer* offer) {
    strncpy(lastOffer.item, offer->item, MAX_ITEM_LEN);
    lastOffer.price = offer->price;
    lastOffer.amount = offer->amount;
}
