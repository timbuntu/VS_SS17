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
    char addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, addr_str, INET_ADDRSTRLEN);

    if(bound) {
        if(listen(sockfd, 4) == 0) {

            std::cout << "HttpServer listening on " << addr_str << ":" << std::to_string(ntohs(addr.sin_port)) << std::endl;
            
            while(bound) {
                sockaddr_storage clientAddr;
                socklen_t clientAddrLen = INET_ADDRSTRLEN;
                int connectionSock = accept(sockfd, (sockaddr*)&clientAddr, &clientAddrLen);
                if(connectionSock >= 0)
                    connectionHandler(connectionSock);
            }
        }
    } else {
        std::cout << "HttpServer could not be bound to " << addr_str << std::endl;
    }
}

void HttpServer::connectionHandler(int sockfd) {
    std::string message = "";
    do {
        char segment[4096];
        recv(sockfd, &segment, 4096, 0);
        std::cout << "Received message: " << std::endl << segment << std::endl;
        message += segment;
    } while(message.find("\r\n\r\n") == string::npos);

    std::string request = message.substr(0, message.find("\r\n"));

    unsigned long endMethod, endURI;
    endMethod = request.find_first_of(" ");
    endURI = request.find_last_of(" ");

    std::string method = request.substr(0, endMethod);
    std::string uri = request.substr(endMethod+1, endURI-(endMethod+1));
    std::string version = request.substr(endURI+1);

    std::string response;
    if(method.compare("GET") != 0)
       response = generateHttpResponse(version, true, "Requested method \"" + method + "\" not implemented");
    else if(uri.compare("/") != 0)
        response = generateHttpResponse(version, true, "Requested URI \"" + uri + "\" not found");
    else
        response = generateHttpResponse(version);

    send(sockfd, response.c_str(), response.length(), 0);
    std::cout << "Message sent" << std::endl;

    close(sockfd);
}

std::string HttpServer::generateHttpResponse(std::string version, bool error, std::string message) const {
    std::string html, header;
    if(error) {
        html = "<html><header><title>Error</title></header><body><h2>Error 404</h2>" + message + "</body></html>";
        header = version + " 404 Not Found\r\n" +
                 "Content-Length: " + std::to_string(html.length()) + "\r\n" +
                 "Content-Type: text/html\r\nConnection: close\r\n\r\n";
    } else {
        html = "<html><header><meta http-equiv=\"refresh\" content=\"1\" /><title>Test</title></header><body>" +
               generateTables() + "</body></html>";
        header = version + " 200 OK\r\n" +
                 "Content-Length: " + std::to_string(html.length()) + "\r\n" +
                 "Content-Type: text/html\r\nConnection: close\r\n\r\n";
    }

    return header + html;
}

std::string HttpServer::generateTables() const {
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
