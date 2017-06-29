/* 
 * File:   MQTTClient.cpp
 * Author: debian
 * 
 * Created on 15. Juni 2017, 11:45
 */

#include "MQTTClient.h"

bool MQTTClient::lib_initialized = false;

MQTTClient::MQTTClient(const char* id, const char* const addr, MosqCallback* callback)
{
    
    if(!lib_initialized) {
        mosquitto_lib_init();
        lib_initialized = true;
    }
    
    this->id = strdup(id);
    client = mosquitto_new(id, true, (void*)callback);
    
    mosquitto_message_callback_set(client, on_message);
    //mosquitto_subscribe_callback_set(client, on_subscribe);
    //mosquitto_log_callback_set(client, on_log);
    
    //mosquitto_threaded_set(client, true);
    //mosquitto_loop_start(client);
    
    int ret = 15;
    ret = mosquitto_connect(client, addr, 1883, 60);
    connected = (ret == MOSQ_ERR_SUCCESS);
    if(connected) {
        printf("%s connected to %s (%d)\n", id, addr, addr);
        loop();
    }
    else
        printf("Connecting to broker at %s failed: %d\n", addr, ret);
    
}

MQTTClient::~MQTTClient() {
    //free(id);
}

bool MQTTClient::subscribe(const char* channel) {
    bool success = false;
    if(connected)
        success = mosquitto_subscribe(client, nullptr, channel, 1) == MOSQ_ERR_SUCCESS;
    else
        printf("Not connected\n");
    
    if(!success)
        printf("Wasn't able to subscribe to %s\n", channel);
    
    return success;
}

bool MQTTClient::publish(const char* channel, void* msg, int len) {
    bool success = false;
    if(connected)
        success = mosquitto_publish(client, nullptr, channel, len, msg, 1, false) == MOSQ_ERR_SUCCESS;
    else
        printf("Not connected\n");
    
    if(!success)
        printf("Wasn't able to publish to %s\n", channel);
    
    return success;
}

void MQTTClient::disconnect() {
    mosquitto_disconnect(client);
    connected = false;
}


void MQTTClient::on_message(struct mosquitto* mosq, void* data, const struct mosquitto_message* msg) {
    //printf("Message received, topic: %s\n", msg->topic);
    ((MosqCallback*)data)->on_mosqEvent(msg->topic, msg->payload);
}

void MQTTClient::on_subscribe(struct mosquitto* mosq, void* data, int mid, int qos_count, const int* granted_qos) {
    printf("Subscribed\n");
}

void MQTTClient::on_log(mosquitto* mosq, void* data, int level, const char* msg) {
    printf("%s\n", msg);
}

void MQTTClient::loop() const {
    //printf("Entering loop\n");
    mosquitto_loop(client, 1000, 1);
    //printf("Leaving loop\n");
}

const char* MQTTClient::getID() const {
    return id;
}

void MQTTClient::addObserver(void(*observer)(void*, int)) {
    observers.push_back(observer);
}

void MQTTClient::notifyObservers(void* msg, int len) const {
    for(void(*observer)(void*, int) : observers)
        observer(msg, len);
}
