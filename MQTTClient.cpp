/* 
 * File:   MQTTClient.cpp
 * Author: debian
 * 
 * Created on 15. Juni 2017, 11:45
 */

#include "MQTTClient.h"

MQTTClient::MQTTClient(const char* id, const char* addr)
{
    client = mosquitto_new(id, true, NULL);
    
    mosquitto_message_callback_set(client, on_message);
    mosquitto_subscribe_callback_set(client, on_subscribe);
    
    mosquitto_threaded_set(client, true);
    mosquitto_loop_start(client);
    
    int ret = mosquitto_connect_async(client, addr, 1883, 60);
    connected = (ret == MOSQ_ERR_SUCCESS);
    if(!connected)
        printf("Connecting to broker at %s failed: %d\n", addr, ret);
    
}

MQTTClient::MQTTClient(const MQTTClient& orig) {
}

MQTTClient::~MQTTClient() {
}

bool MQTTClient::subscribe(char* channel) {
    bool success = false;
    if(connected)
        success = mosquitto_subscribe(client, nullptr, channel, 0) == MOSQ_ERR_SUCCESS;
    else
        printf("Not connected\n");
    
    return success;
}

bool MQTTClient::publish(char* channel, void* msg, int len) {
    bool success = false;
    if(connected)
        success = mosquitto_publish(client, nullptr, channel, len, msg, 0, true) == MOSQ_ERR_SUCCESS;
    else
        printf("Not connected\n");
    
    return success;
}

void MQTTClient::on_message(struct mosquitto* mosq, void* data, const struct mosquitto_message* msg) {
    //notifyObservers(msg->payload, msg->payloadlen);
    printf("Message received");
}

void MQTTClient::on_subscribe(struct mosquitto* mosq, void* data, int mid, int qos_count, const int* granted_qos) {
    printf("Subscribed\n");
}


void MQTTClient::addObserver(void(*observer)(void*, int)) {
    observers.push_back(observer);
}

void MQTTClient::notifyObservers(void* msg, int len) const {
    for(void(*observer)(void*, int) : observers)
        observer(msg, len);
}
