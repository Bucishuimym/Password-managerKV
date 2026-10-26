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
    //取值的接口都加 const：链表按字段查、打印 const& 记录时都要用
    std::string GetUsername() const;
    std::string GetKey() const;
    std::string GetUrl() const;
    int GetUid() const;
    void SetUsername( std::string _Username);
    void SetKey(std::string _key);
    void SetUrl(std::string _url);
    void SetUid(int _uid);
    void input();
    void inPut();
};

//密码数据表定义在 OverAllkv.cpp 里
extern DoubleList<pwDataGroup> pwdata;

#endif // !PWDATAGROUP_H
