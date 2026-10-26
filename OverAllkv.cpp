#include <iostream>
#include <string>
#include  "pwDataGroup.h"

#pragma comment(lib,"DoubleLinkedList.lib")

static int LocatePasswordBySeq(const int& ID)
{
	pwDataGroup newData;
	std::string ps = "";
	if (pwdata.LocateByUid(ID)  != newData.GetUid()) {
		return -1;
	}
}//按 UID 在密码表查找，找不到返回 -1