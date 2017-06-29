/* 
 * File:   Producer.h
 * Author: debian
 *
 * Created on 15. Juni 2017, 17:07
 */

#ifndef PRODUCER_H
#define	PRODUCER_H

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "MQTTClient.h"
#include "MosqCallback.h"

#define OFFER_DELAY 10

class Producer : public MosqCallback {
public:
    Producer(const char* id, const char* const addr, const char** products, unsigned int n);
    virtual ~Producer();
    
    void start();
    void stop();
    
    void on_mosqEvent(const char* channel, const void* msg);
    
private:
    
    void saveOffer(MQTTClient::Offer* offer);
    
    MQTTClient* client;
    MQTTClient::Offer lastOffer;
    const char** products;
    unsigned int productCount;
    unsigned long loopCount;
    bool stopProducer;
};

#endif	/* PRODUCER_H */

