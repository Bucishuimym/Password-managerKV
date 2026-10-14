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
#pragma comment(lib,"DLinkedList.lib")

std::vector<UserGroup> usr;
DoubleList<UserGroup> users(usr.data(), (int)Max_User);

UserGroup::UserGroup() :name(), key(), uid(), rule() {}
UserGroup::UserGroup(std::string _name, std::string _key, int _uid, int _rule) :
	name(_name), key(_key), uid(_uid), rule(_rule) {}
UserGroup::~UserGroup() = default;

std::string UserGroup::GetName() const { return name; }
std::string UserGroup::GetKey() const{ return key; }
int			UserGroup::GetRule() const { return rule; }
int			UserGroup::GetUid() const { return uid; }

void UserGroup::SetName(std::string _name) { name = std::move(_name); }
void UserGroup::SetKey(std::string _key)   { key = std::move(_key); }
void UserGroup::SetUid(int _uid)           { uid = std::move(_uid); }
void UserGroup::SetRule(int _rule)         { rule = std::move(_rule); }
