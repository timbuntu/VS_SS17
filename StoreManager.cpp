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
        itemStock.insert(std::pair<std::string, int>(items[i], INITIAL_STOCK));
        offersSent.insert(std::pair<std::string, int>(items[i], 0));
        client.subscribe(std::string(OFFER_PREFIX + items[i]).c_str());
    }
    client.subscribe(CONFIRM_CHANNEL);
    
    serverThread = new std::thread(StoreHandler::startStoreServer, port, &itemPrices, &itemStock, &orders);
}

StoreManager::~StoreManager() {
    delete serverThread;
}

void StoreManager::startLoop() {
    
    while(true) {
        client.loop();
        for(auto i = itemStock.cbegin(); i != itemStock.cend(); i++) {
            std::string item = i->first;
            unsigned int stock = i->second;
            
            if(stock < INITIAL_STOCK*0.8) {
                if(offersSent.at(item) % OFFER_DELAY == 0) {
                    MQTTClient::Offer offer;
                    strncpy(offer.item, item.c_str(), MAX_ITEM_LEN);
                    offer.price = itemPrices.at(item)/2 + (offersSent.at(item)/OFFER_DELAY)*PRICE_INCREASE;
                    offer.amount = INITIAL_STOCK - stock;
                    client.publish(std::string(DEMAND_PREFIX + item).c_str(), &offer, sizeof(offer));
                    printf("%s: Sent demand for %d %s\n", client.getID(), offer.amount, offer.item);
                }
                offersSent.at(item)++;
            }
        }
        //printf("Store alive\n");
    }
    
    serverThread->join();
}

void StoreManager::on_mosqEvent(const char* channel, const void* msg) {
    MQTTClient::Offer* offer = (MQTTClient::Offer*)msg;
    //printf("Offer: %d times %s for %d each\n", offer->amount, offer->item, offer->price);
    if(strstr(channel, OFFER_PREFIX)) {
        const char* item = channel + strlen(OFFER_PREFIX);
        //printf("Received offer for %s\n", item);
        if(!strcmp(item, offer->item) && itemStock.at(item) < INITIAL_STOCK && !isActiveOrder(offer) && offer->price <= (itemPrices.at(item)/2 + (offersSent.at(item)/OFFER_DELAY)*PRICE_INCREASE)) {
            
            MQTTClient::Offer *order;
            if(offer->amount <= INITIAL_STOCK-itemStock.at(item))
                order = offer;
            else {
                order = new MQTTClient::Offer;
                strncpy(order->item, item, MAX_ITEM_LEN);
                order->price = offer->price;
                order->amount = INITIAL_STOCK-itemStock.at(item);
            }
            if(client.publish(ORDER_CHANNEL, order, sizeof(*order))) {
                
                strncpy(activeOrder.item, order->item, MAX_ITEM_LEN);
                activeOrder.price = order->price;
                activeOrder.amount = order->amount;
                
                if(order->amount != offer->amount)
                    delete order;
                printf("%s: Received offer for and ordered %d %s\n", client.getID(), order->amount, order->item);
            }
        }
    } else if(strstr(channel, CONFIRM_CHANNEL)) {
        if(!strncmp(offer->item, activeOrder.item, MAX_ITEM_LEN) && offer->price == activeOrder.price && offer->amount == activeOrder.amount) {
            itemStock.at(offer->item) += offer->amount;
            offersSent.at(offer->item) = 0;
            *activeOrder.item = 0;
            printf("%s: Restocked %s by %d to %d\n", client.getID(), offer->item, offer->amount, itemStock.at(offer->item));
        }
    } else {
        printf("%s: Ignored offer for %d %s\n", client.getID(), offer->amount, offer->item);
    }
}

bool StoreManager::isActiveOrder(MQTTClient::Offer* offer) const {
    bool equal = true;
    
    if(strncmp(activeOrder.item, offer->item, MAX_ITEM_LEN))
        equal = false;
    else if(activeOrder.price != offer->price)
        equal = false;
    else if(activeOrder.amount != offer->amount)
        equal = false;
    
    return equal;
}
