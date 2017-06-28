/* 
 * File:   MQTTClient.h
 * Author: debian
 *
 * Created on 15. Juni 2017, 11:45
 */

#ifndef MQTTCLIENT_H
#define	MQTTCLIENT_H

#include <cstdio>
#include <vector>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <mosquitto.h>
#include "MosqCallback.h"

#define ORDER_CHANNEL "order"
#define CONFIRM_CHANNEL "confirm"
#define DEMAND_PREFIX "demand/"
#define OFFER_PREFIX "offer/"
#define MAX_ITEM_LEN 16

class MQTTClient {
public:
    MQTTClient(const char* id, const char* const addr, MosqCallback* callback);
    virtual ~MQTTClient();
    
    bool subscribe(const char* channel);
    bool publish(const char* channel, void* msg, int len);
    
    static void on_message(struct mosquitto* mosq, void* data, const struct mosquitto_message* msg);
    static void on_subscribe(struct mosquitto* mosq, void* data, int mid, int qos_count, const int * granted_qos);
    static void on_log(mosquitto* mosq, void* data, int level, const char* msg);
    
    void addObserver(void (*observer)(void*, int));
    void notifyObservers(void* msg, int len) const;
    
    void loop() const;
    
    const char* getID() const;
    
    struct Offer {
        char item[MAX_ITEM_LEN];
        unsigned int price;
        unsigned int amount;
    };
    
private:
    static bool lib_initialized;
    char* id;
    bool connected;
    mosquitto* client;
    std::vector<void(*)(void*, int)> observers;
};

#endif	/* MQTTCLIENT_H */

