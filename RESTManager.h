/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RESTManager.h
 * Author: tim
 *
 * Created on 18. Mai 2017, 11:45
 */

#ifndef RESTMANAGER_H
#define RESTMANAGER_H

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstdlib>
#include <string.h>
#include <iostream>
#include <fstream>
#include <list>

#define RES_FOLDER_NAME ((std::string)"res")
#define INDEX_NAME ((std::string)"index")

using namespace std;

class RESTManager {
public:
    RESTManager();
    RESTManager(string resources[], unsigned int length);
    RESTManager(const RESTManager&);
    virtual ~RESTManager();
    
    void initStructure() const;
    list<string> get(string res) const;
    void put(string res, string value, bool append = false) const;
    
private:
    string* resources;
    unsigned int number;
};

#endif /* RESTMANAGER_H */

