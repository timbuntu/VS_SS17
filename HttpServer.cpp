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

HttpServer::HttpServer(sockaddr_in addr, RESTManager manager) {
    this->manager = manager;
    this->addr = addr;
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
            char addr_str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &addr.sin_addr, addr_str, INET_ADDRSTRLEN);
            std::cout << "HttpServer listening on " << addr_str << ":" << std::to_string(ntohs(addr.sin_port)) << std::endl;
            
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
    std::cout << "Received message: " << std::endl << message << std::endl;
    std::string html = "<html><header><meta http-equiv=\"refresh\" content=\"1\" /><title>Test</title></header><body>" + createTables() + "</body></html>";
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(html.length()) + "\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" + html;
    
    if(string(message).find("GET") != string::npos) {
        send(sockfd, response.c_str(), response.length(), 0);
        std::cout << "Message sent" << std::endl;
    }
    close(sockfd);
}

std::string HttpServer::createTables() const {
    std::string table = "<h3>Aktuell</h3><table><tr><th>Produkt</th><th>Menge</th></tr>";
    std::list<string> items = manager.get(INDEX_NAME);
    items.remove("history");
    for(string item : items) {
        if(manager.get(item).size() > 0) {
            table += "<tr><td>" + item + "</td><td>" + manager.get(item).front() + "</td></tr>";
             if (table.find("ä") != string::npos)
                table.replace(table.find("ä"), 2, "&auml;");
        }
    }
    table += "</table>";
    
    table += "<h3>History</h3><table><tr><th>Produkt</th><th>Menge</th></tr>";
    std::list<string> history = manager.get("history");
    for(string entry : history) {
        int pos = entry.find('=');
        string name = entry.substr(0, pos);
        string amount = entry.substr(pos+1, entry.length()-pos);
        
        table += "<tr><td>" + name + "</td><td>" + amount + "</td></tr>";
        if (table.find("ä") != string::npos)
                table.replace(table.find("ä"), 2, "&auml;");
    }
    table += "</table>";
    
    return table;
}
