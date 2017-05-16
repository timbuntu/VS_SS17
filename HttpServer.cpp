/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HttpServer.cpp
 * Author: tim
 * 
 * Created on 16. Mai 2017, 12:29
 */

#include "HttpServer.h"

HttpServer::HttpServer(sockaddr_in addr) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(sockfd >= 0) {
        int optionValue = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(int));
        bound = (bind(sockfd, (sockaddr*)&addr, sizeof(sockaddr_in)) == 0);
    }
}

HttpServer::~HttpServer() {
    close(sockfd);
}

void HttpServer::start() {
    if(bound) {
        if(listen(sockfd, 4) == 0) {
            while(bound) {
                sockaddr_storage clientAddr;
                socklen_t clientAddrLen = INET_ADDRSTRLEN;
                int connectionSock = accept(sockfd, (sockaddr*)&clientAddr, &clientAddrLen);
                if(connectionSock >= 0)
                    connectionHandler(connectionSock, (sockaddr_in*)&clientAddr);
            }
        }
    }
}

void HttpServer::connectionHandler(int sockfd, sockaddr_in* clientAddr) {
    char message[4096];
    recv(sockfd, &message, 4096, 0);
    std::string html = "<html><header><meta http-equiv=\"refresh\" content=\"5\" /><title>Test</title></header><body>" + createTable() + "</body></html>";
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(html.length()) + "\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" + html;
    send(sockfd, response.c_str(), response.length(), 0);
    std::cout << "Message sent" << std::endl;
    close(sockfd);
}

std::string HttpServer::createTable() const {
    std::string table = "<table><tr><th>Name</th><th>Menge</th></tr>";
    
}
