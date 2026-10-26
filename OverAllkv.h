#pragma once
#ifndef OVERALLKV_H
#define OVERALLKV_H

#include <string>
#include <iostream>

#include "DLinkedList.h"
#include "pwDataGroup.h"
#include "UserGroup.h"

#include "DoubleLinkedList/DoubleLinkedList.cpp"	//模板实现得跟调用方一起编译，链表的方法都在这里

const int Max_User = 1000;
const int Max_Password = 1000;
const int RULE_ADMIN = 2;
const int RULE_USER = 1;

extern int userCount;  //用户数量
extern int pasCount;   //表长度
extern int ruletoken;  //当前登录用户的权限，-1 表示未登录
extern int userUid;    //记录当前登录用户的UID，登出重置000

const std::string USER_FILE = "users.txt";
const std::string DATA_FILE = "data.txt";

//两张表，定义在 OverAllkv.cpp；链表自己带容量上限，满了 PushBack / Insert 返回 false
extern DoubleList<UserGroup> users;
extern DoubleList<pwDataGroup> pwdata;

std::string getInput(const std::string& prompt);
void initAdmin();                                                              //初始化管理员账号
int login(int& outRule);                                                       //登录，成功把权限写回 outRule
void signUp();                                                                 //注册新用户
void adminMenu();                                                              //管理员菜单
void userMenu();                                                               //普通用户菜单
int uidCount();                                                                //生成下一个用户 UID
std::string numToStr(int num);                                                 //仅声明、暂无定义：数字转字符串
int strToNum(std::string str);                                                 //仅声明、暂无定义：字符串转数字
void encryptDecrypt(char* str);                                                //对称加解密（异或 0x7F）
int hexVal(char c);                                                            //单个十六进制字符转数值
std::string toHex(const std::string& s);                                       //密文转十六进制字符串（写文件、打印用）
std::string fromHex(const std::string& s);                                     //十六进制字符串转回密文（读文件用）
void outFile();                                                                //将数据覆盖到文件
void inFile();                                                                 //将数据从文件读入

int Length();                                                                  //返回密码表长度
int LocatePasswordBySeq(const std::string& username);                          //仅声明、暂无定义（重载：按用户名查密码表）
int LocateUserByUsers(const std::string& username);                            //按用户名在用户组查找，找不到返回 -1
int LocateUserByUid(int uid);                                                  //按 UID 在用户组查找，找不到返回 -1
int LocatePasswordBySeq(const int& ID);                                        //按 UID 在密码表查找，找不到返回 -1
int LocateMyDataBySeq(int seq);                                                //我的第 seq 条 → 表里绝对下标
int PasswordData(int index);                                                   //仅声明、暂无定义（实际用的是 getPasswordData）
int Empty();                                                                   //密码表是否为空（空返回 1）
void PrintLineData(const pwDataGroup& item, bool showUid = true);              //打印一行密码数据
void PrintLineUser(const UserGroup& u);                                        //打印一行用户数据
void ListData();                                                               //列出数据：admin 列全部，user 只列自己的
void searchPassword();                                                         //仅声明、暂无定义：搜索密码
void PrintUserData();                                                          //打印用户组所有信息

#endif
