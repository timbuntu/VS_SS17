/* 
 * File:   MQTTClient.cpp
 * Author: debian
 * 
 * Created on 15. Juni 2017, 11:45
 */

#include "MQTTClient.h"

MQTTClient::MQTTClient(const char* addr) {
    mosqpp::lib_init();
    connected = (connect_async(addr) == MOSQ_ERR_SUCCESS);
    if(!connected)
        printf("Connecting to broker failed\n");
}

MQTTClient::MQTTClient(const MQTTClient& orig) {
}

MQTTClient::~MQTTClient() {
}

bool MQTTClient::subscribe(char* channel) {
    bool success = false;
    if(connected)
        success = mosqpp::mosquittopp::subscribe(nullptr, channel) == MOSQ_ERR_SUCCESS;
    else
        printf("Not connected\n");
    
    return success;
}

bool MQTTClient::publish(char* channel, void* msg, int len) {
    bool success = false;
    if(connected)
        success = mosqpp::mosquittopp::publish(nullptr, channel, len, msg) == MOSQ_ERR_SUCCESS;
    else
        printf("Not connected\n");
    
    return success;
}

void MQTTClient::on_message(const struct mosquitto_message* msg) {
    notifyObservers(msg->payload, msg->payloadlen);
}

void MQTTClient::on_subscribe(int mid, int qos_count, const int* granted_qos) {
    printf("Subscribed\n");
}


void MQTTClient::addObserver(void(*observer)(void*, int)) {
    observers.push_back(observer);
}

void MQTTClient::notifyObservers(void* msg, int len) const {
    for(void(*observer)(void*, int) : observers)
        observer(msg, len);
}
