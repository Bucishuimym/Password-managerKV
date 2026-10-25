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

pwDataGroup::pwDataGroup() :Username(), key(), url(), uid() {}
pwDataGroup::pwDataGroup(std::string _Username, std::string _key, std::string _url, int _uid) :Username(_Username), key(_key), url(_url), uid(_uid){}
pwDataGroup::~pwDataGroup() = default;

std::string pwDataGroup::GetUsername() { return Username; }
std::string pwDataGroup::GetKey() { return key; }
std::string pwDataGroup::GetUrl() { return url; }
int pwDataGroup::GetUid() { return uid; }
void pwDataGroup::SetUsername(std::string _Username) { Username = std::move(_Username); }
void pwDataGroup::SetKey(std::string _key) { key = std::move(_key); }
void pwDataGroup::SetUrl(std::string _url) { url = std::move(_url); }
void pwDataGroup::SetUid(int _uid) { uid = std::move(uid); }
