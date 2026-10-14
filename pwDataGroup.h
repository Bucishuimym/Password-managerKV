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

#include "pwDataGroup.h"
#include "UserGroup.h"
#include "OverAllkv.h"
#include "DLinkedList.h"
#pragma comment(lib,"DLinkedList.lib")

class pwDataGroup{
private:
    std::string Username;
    std::string key;
    std::string url;
    int uid;
public:
    pwDataGroup();
    pwDataGroup(std::string us, std::string k, std::string ur, int ui);
    ~pwDataGroup();
    std::string GetUsername();
    std::string GetKey();
    std::string GetUrl();
    int GetUid();
    void SetUsername();
    void SetKey();
    void SetUrl();
    void SetUid();
};

#endif // !PWDATAGROUP_H
