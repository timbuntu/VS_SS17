/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RESTManager.cpp
 * Author: tim
 * 
 * Created on 18. Mai 2017, 11:45
 */

#include <vector>
#include <algorithm>
#include <list>

#include "RESTManager.h"

RESTManager::RESTManager() {
    resources = NULL;
    number = 0;
}


RESTManager::RESTManager(string resources[], unsigned int length) {
    this->resources = resources;
    number = length;
}

RESTManager::RESTManager(const RESTManager& orig) {
    resources = orig.resources;
    number = orig.number;
}


RESTManager::~RESTManager() {
}

void RESTManager::initStructure() const {
    DIR* dir = opendir(RES_FOLDER_NAME.c_str());
    if(dir == NULL) {
        mkdir(RES_FOLDER_NAME.c_str(), 0770);
        dir = opendir(RES_FOLDER_NAME.c_str());
    }
    while(dirent* file = readdir(dir)) {
        if(strcmp(file->d_name, ".") && strcmp(file->d_name, ".."))
            remove(((string)(RES_FOLDER_NAME + "/" + string(file->d_name))).c_str());
    }
    ofstream out(RES_FOLDER_NAME + "/" + INDEX_NAME);
    for(int i = 0; i < number; i++)
        out << resources[i] << endl;
    out.close();
}

list<string> RESTManager::get(string res) const {
    ifstream inf;
    list<string> values;
    do {
        inf.open(RES_FOLDER_NAME + "/" + res);
        while(inf.good() && !inf.eof()) {
            string line = "";
            inf >> line;
            values.push_front(line);
        }
        //cout << "FAIL " << res << endl;
    } while(inf.bad());
    inf.close();
    values.remove("");
    return values;
}

void RESTManager::put(string res, string value, bool append) const {
    ofstream outf;
    if(append)
        outf.open(RES_FOLDER_NAME + "/" + res, ios::out | ios::app);
    else
        outf.open(RES_FOLDER_NAME + "/" + res);
    
    outf << value << endl;
    outf.close();
}
