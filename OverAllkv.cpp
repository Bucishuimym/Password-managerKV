#include <iostream>
#include <string>

#include "OverAllkv.h"
#include "pwDataGroup.h"
#include "UserGroup.h"
#include "DLinkedList.h"

#pragma comment(lib,"DoubleLinkedList.lib")

//两张表和各全局变量的定义都收在这里（头文件里只放声明）
DoubleList<UserGroup> users((int)Max_User);
DoubleList<pwDataGroup> pwdata((int)Max_Password);

int userCount = 0;  //用户数量
int pasCount = 0;   //表长度
int ruletoken = -1; //当前登录用户的权限，-1 表示未登录
int userUid = 000;  //记录当前登录用户的UID，登出重置000

//下面几个定位函数返回的是"下标"（跟以前数组一样从 0 开始），找不到返回 -1
//所以调用方拿去用链表的 Get / DeleteByPos 时，位序要 +1（位序从 1 开始）
int LocateUserByUsers(const std::string& username) {                           //按用户名在用户组查找
    int index = 0;
    for (auto* p = users.Begin(); !users.AtEnd(p); p = users.Next(p)) {
        if (p->data.GetName() == username)
            return index;
        index++;
    }
    return -1;
}

int LocateUserByUid(int uid) {                                                 //按 UID 在用户组查找
    int index = 0;
    for (auto* p = users.Begin(); !users.AtEnd(p); p = users.Next(p)) {
        if (p->data.GetUid() == uid)
            return index;
        index++;
    }
    return -1;
}

int LocatePasswordBySeq(const int& ID) {                                       //按 UID 在密码表查找
    int index = 0;
    for (auto* p = pwdata.Begin(); !pwdata.AtEnd(p); p = pwdata.Next(p)) {
        if (p->data.GetUid() == ID)
            return index;
        index++;
    }
    return -1;
}

//把"我的第 seq 条"还原成密码表里的下标，找不到返回 -1
//过滤条件和 ListData() 给用户编号时用的是同一套，保证"看到几号就能删几号"
int LocateMyDataBySeq(int seq) {                                               //我的第 seq 条 → 表里下标
    int shown = 0, index = 0;
    for (auto* p = pwdata.Begin(); !pwdata.AtEnd(p); p = pwdata.Next(p)) {
        if (p->data.GetUid() != userUid) {
            index++;
            continue;               //不是我的数据，不占号
        }
        if (++shown == seq)
            return index;
        index++;
    }
    return -1;
}

int Length() {                                                                 //返回密码表长度
    return pwdata.Length();     //表长由链表自己维护，不用再单独数
}

int Empty() {                                                                  //密码表是否为空（空返回 1）
    return pwdata.Empty() ? 1 : 0;
}

//打印一条密码数据：key 同时给出密文（表里存的）和明文（解密后的）
//showUid 默认 true；普通用户看自己的数据时传 false，不把这条数据属于谁暴露出去
void PrintLineData(const pwDataGroup& item, bool showUid) {                    //打印一行密码数据
    std::string showkey = item.GetKey();     //拷贝一份来解密，不动表里存的密文
    if (!showkey.empty())
        encryptDecrypt(&showkey[0]);
    std::cout << "|————" << item.GetUsername() << '\n'
        << "  |——" << item.GetUrl() << '\n'
        << "  |——key:" << toHex(item.GetKey()) << "(" << showkey << ")" << std::endl;
    if (showUid)
        std::cout << "  |_UID:" << item.GetUid() << std::endl;
    std::cout << std::endl;
}

void PrintUserData() {                                                         //打印用户组所有信息
    if (ruletoken != RULE_ADMIN) {
        std::cout << "[INFO] - Rule error!\n";
        system("pause");
        return;
    }
    users.ForEach([](const UserGroup& u) { PrintLineUser(u); });  //只读遍历，正好走 ForEach
}

//列出数据，分两层：先看当前令牌，admin 直接列出全部用户的数据，
//user 则按全局变量 userUid 匹配，只列出跟自己相关的那几条
//user 看到的每行前面带一个从 1 开始的序号，删除时输的就是它（见 LocateMyDataBySeq）
void ListData() {                                                              //列出数据：admin 列全部，user 只列自己的
    bool seeAll = (ruletoken == RULE_ADMIN);
    int shown = 0;
    for (auto* p = pwdata.Begin(); !pwdata.AtEnd(p); p = pwdata.Next(p)) {
        if (!seeAll && p->data.GetUid() != userUid)
            continue;                       //不是自己的数据直接跳过，连 UID 都不露出来
        if (!seeAll)
            std::cout << "[" << shown + 1 << "] ";   //这个号只在"user自己的数据"里连续，不是表里的下标
        PrintLineData(p->data, seeAll);    //admin 带 UID，user 不带
        shown++;
    }
    if (shown == 0)
        std::cout << (seeAll ? "数据表是空的。" : "你还没有存储任何数据。") << std::endl;
    else
        std::cout << "共 " << shown << " 条。" << std::endl;
}
