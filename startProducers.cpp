
#include <thread>
#include <iostream>

#include "RESTManager.h"
#include "Producer.h"


using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    mosqpp::lib_init();
    
    RESTManager manager(NULL, 0);
    manager.initStructure();
    
    const char* addr = manager.getConfig("BrokerIp").c_str();
    char* farmProducts[] = {"Milk", "Cheese"};
    char* marketProducts[] = {"Bread", "Juice"};
    
    Producer farm1("farm1", addr, farmProducts, 2);
    Producer farm2("farm2", addr, farmProducts, 2);
    Producer market1("markt1", addr, marketProducts, 2);
    Producer market2("markt2", addr, marketProducts, 2);
    
    thread tFarm1(&Producer::start, &farm1);
    thread tFarm2(&Producer::start, &farm2);
    thread tMarket1(&Producer::start, &market1);
    thread tMarket2(&Producer::start, &market2);
    
    tFarm1.join();
    tFarm2.join();
    tMarket1.join();
    tMarket2.join();
    
    printf("End\n");
    
    return 0;
}