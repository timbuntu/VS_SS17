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

#include <mosquittopp.h>

#define DEMAND_PREFIX "demand/"
#define OFFER_PREFIX "offer/"

class MQTTClient : public mosqpp::mosquittopp {
public:
    MQTTClient(const char* addr);
    MQTTClient(const MQTTClient& orig);
    virtual ~MQTTClient();
    
    bool subscribe(char* channel);
    bool publish(char* channel, void* msg, int len);
    
    void on_message(const struct mosquitto_message* msg);
    void on_subscribe(int mid, int qos_count, const int * granted_qos);
    
    void addObserver(void (*observer)(void*, int));
    void notifyObservers(void* msg, int len) const;
    
private:
    bool connected;
    std::vector<void(*)(void*, int)> observers;
};

#endif	/* MQTTCLIENT_H */

