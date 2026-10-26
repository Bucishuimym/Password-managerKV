#pragma once

#ifndef USERGROUP_H
#define USERGROUP_H

#include <cstring>
#include <iostream>
#include <string>
#include <cstdlib>
#include <windows.h>
#include <fstream>
#include <sstream>

#include "DLinkedList.h"
#pragma comment(lib,"DoubleLinkedList.lib")

class UserGroup{
private:
    std::string name;
    std::string key;
    int uid;
    int rule;
public:
    UserGroup();
    UserGroup(std::string _name,std::string _key,int _uid,int _rule);
    ~UserGroup();
    std::string GetName() const;
    std::string GetKey() const;
    int GetUid() const;
    int GetRule() const;
    void SetName(std::string name);
    void SetKey(std::string key);
    void SetUid(int uid);
    void SetRule(int rule);
};

//用户表定义在 OverAllkv.cpp 里
extern DoubleList<UserGroup> users;
#endif // !1

