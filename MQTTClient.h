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

#include <mosquitto.h>
#include "MosqCallback.h"

#define DEMAND_PREFIX "demand/"
#define OFFER_PREFIX "offer/"
#define MAX_ITEM_LEN 16

class MQTTClient {
public:
    MQTTClient(const char* id, const char* const addr, MosqCallback* callback);
    MQTTClient(const MQTTClient& orig);
    virtual ~MQTTClient();
    
    bool subscribe(char* channel);
    bool publish(char* channel, void* msg, int len);
    
    static void on_message(struct mosquitto* mosq, void* data, const struct mosquitto_message* msg);
    static void on_subscribe(struct mosquitto* mosq, void* data, int mid, int qos_count, const int * granted_qos);
    static void on_log(mosquitto* mosq, void* data, int level, const char* msg);
    
    void addObserver(void (*observer)(void*, int));
    void notifyObservers(void* msg, int len) const;
    
    void loop() const;
    
    struct Offer {
        char item[MAX_ITEM_LEN];
        unsigned int price;
        unsigned int amount;
    };
    
private:
    bool connected;
    mosquitto* client;
    std::vector<void(*)(void*, int)> observers;
};

#endif	/* MQTTCLIENT_H */

