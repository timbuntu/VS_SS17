/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   startServer.cpp
 * Author: tim
 *
 * Created on 20. April 2017, 20:42
 */

#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Server.h"

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 27015

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    addr.sin_port = htons(SERVER_PORT);
    
    Server server(addr);
    server.receive();
    
    return 0;
}

