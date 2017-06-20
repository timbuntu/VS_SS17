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


class Producer : public MosqCallback {
public:
    Producer(const char* id, const char* const addr, const char** products, unsigned int n);
    virtual ~Producer();
    
    void start();
    
    void on_mosqEvent(const void* msg);
    
private:
    MQTTClient* client;
    const char** products;
    unsigned int n;
};

#endif	/* PRODUCER_H */

