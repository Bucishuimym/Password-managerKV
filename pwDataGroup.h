#pragma once

#ifndef PWDATAGROUP_H
#define PWDATAGROUP_H

#include <cstring>
#include <iostream>
#include <string>
#include <cstdlib>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <vector>

#include "pwDataGroup.h"
#include "UserGroup.h"
#include "OverAllkv.h"
#include "DLinkedList.h"
#pragma comment(lib,"DoubleLinkedList.lib")

class pwDataGroup{
private:
    std::string Username;
    std::string key;
    std::string url;
    int uid;
public:
    pwDataGroup();
    pwDataGroup(std::string _Username, std::string _key, std::string _url, int _uid);
    ~pwDataGroup();
    std::string GetUsername();
    std::string GetKey();
    std::string GetUrl();
    int GetUid();
    void SetUsername( std::string _Username);
    void SetKey(std::string _key);
    void SetUrl(std::string _url);
    void SetUid(int _uid);
};

extern std::vector<pwDataGroup> pw;
extern DoubleList<pwDataGroup> pwdata;

#endif // !PWDATAGROUP_H
