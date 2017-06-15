/* 
 * File:   Producer.h
 * Author: debian
 *
 * Created on 15. Juni 2017, 17:07
 */

#ifndef PRODUCER_H
#define	PRODUCER_H

#include <string.h>
#include "MQTTClient.h"


class Producer {
public:
    Producer(const char* addr, char** products, unsigned int n);
    virtual ~Producer();
    
    void start();
    
private:
    MQTTClient* client;
    char** products;
    unsigned int n;
};

#endif	/* PRODUCER_H */

