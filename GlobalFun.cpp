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

using namespace std;

void initAdmin() {                                                             //初始化管理员账号
    users[0].name = "admin";
    users[0].key = "123";
    users[0].rule = RULE_ADMIN;
    users[0].uid = uidCount();
    if (!users[0].key.empty())
    {
        encryptDecrypt(&users[0].key[0]);
    }
    userCount = 1;
}

int uidCount() {                                                               //生成下一个用户 UID
    int usC = userCount;
    int num = usC++;
    int UID = (num + 234565) % 1000 + 9000;
    return UID;
}

int login(int& outRule) {                                                      //登录，成功写回权限令牌
    userUid = 000;
    string inputName, inputKey;
    system("cls");
    cout << "============== User Login ==============" << endl;

    cout << "Enter user name: "; cin >> inputName;
    cout << "Enter user key: "; cin >> inputKey;

    string key = inputKey;      //输入的是明文，先加密再跟表里的密文比
    if (!key.empty())
        encryptDecrypt(&key[0]);

    int i = LocateUserByUsers(inputName);   //先用函数按用户名定位
    if (i == -1) {                          //先判查不到：i 是 -1 时不能拿去索引 users，会越界读到表外
        cout << "\nUsername no found!" << endl;
        system("pause");
        return -1;
    }
    if (users[i].key != key) {
        cout << "\nPassword error!" << endl;
        system("pause");
        return -1;
    }
    cout << "\nLogin Successful Hello," << users[i].name << "\n" << endl;
    outRule = users[i].rule;
    userUid = users[i].uid;
    PrintLineUser(users[i]);    //登录成功，把这行用户信息打出来
    system("pause");
    return 1;
}

void signUp() {                                                                //注册新用户
    system("cls");
    if (userCount >= Max_User) {
        cout << "Sorry! user max" << endl;
        system("pause");
        return;
    }

    string name, key;
    cout << "============== Sign up ==============" << endl;
    cout << "Enter new user name: ";
    cin >> name;

    if (LocateUserByUsers(name) != -1) {
        cout << "该用户名已存在，注册失败！" << endl;
        system("pause");
        return;
    }

    cout << "Enter new user key: ";
    cin >> key;
    if (!key.empty())
    {
        encryptDecrypt(&key[0]);
    }
    users[userCount].name = name;
    users[userCount].key = key;
    users[userCount].rule = RULE_USER;
    users[userCount].uid = uidCount();

    userCount++;

    cout << "Registration succeeded!" << endl;
    system("pause");
}

void adminMenu() {                                                             //管理员菜单
    int choice;
    while (true) {
        system("cls");
        cout << "+================= Admin Menu ==================+" << endl;
        cout << "| [1].按UID查询密码数据                         |" << endl;
        cout << "| [2].按用户名查询用户                          |" << endl;
        cout << "| [3].插入密码数据                              |" << endl;
        cout << "| [4].插入用户数据                              |" << endl;
        cout << "| [5].按位置删除密码数据                        |" << endl;
        cout << "| [6].按位置/UID删除用户数据                    |" << endl;
        cout << "| [7].打印全部密码数据                          |" << endl;
        cout << "| [8].打印全部用户数据                          |" << endl;
        cout << "| [9].退出登录                                  |" << endl;
        cout << "+-----------------------------------------------+" << endl;
        cout << "Enter your choice: ";
        cin >> choice;      //输入要放在循环里，不然 choice 一直是第一次的值，菜单会死循环
        if (!cin) {
            bool atEof = cin.eof();     //要在 clear() 之前判断，clear() 会把 eofbit 一起清掉
            cin.clear();
            cin.ignore(1000, '\n');
            if (atEof)
                return;                 //输入流到头了（管道/重定向），直接退出
            cout << "Invalid input, please enter a number!" << endl;
            system("pause");
            continue;
        }

        switch (choice) {
        case 1: {  //[1].LocateData
            int id;
            cout << "Enter a UID:" << endl;
            cin >> id;
            int pos = LocatePasswordBySeq(id);
            if (pos != -1) {
                string showkey = Slist[pos].key;
                if (!showkey.empty())
                    encryptDecrypt(&showkey[0]);
                cout << "|————" << Slist[pos].Username << "\n"
                    << "  |——" << Slist[pos].URL << "\n"
                    << "  |——key:" << toHex(Slist[pos].key) << "(" << showkey << ")" << "\n"
                    << "  |_" << Slist[pos].UID << endl
                    << endl;
            }
            system("pause");
            break;
        }
        case 2: { //[2].LocateUserdata
            string name;
            cin >> name;
            int pos = LocateUserByUsers(name);
            if (pos != -1) {
                string showkey = users[pos].key;
                if (!showkey.empty())
                    encryptDecrypt(&showkey[0]);
                cout << "|————" << users[pos].name << "\n"
                    << "  |——key:" << toHex(users[pos].key) << "(" << showkey << ")" << endl
                    << "  |_" << users[pos].uid << endl
                    << endl;
            }
            system("pause");
            break;
        }
        case 3: {  //[3].InsertData
            int n;
            cout << "Input number of pos: ";
            cin >> n;
            PasswordItem item;
            cout << "Enter your data(name;URL;key): ";
            cin >> item.Username >> item.URL >> item.key;
            item.UID = userUid;
            encryptDecrypt(&item.key[0]);
            Insert(Slist, pasCount, n, item, Max_Password);
            break;
        }
        case 4: { //[4].InsertUserdata
            signUp();
            //Insert(users,userCount,n,user,Max_User);
            break;
        }
        case 5: {  //[5].DeleteData
            int index;
            cout << "Input index: ";
            cin >> index;
            PasswordItem x = Delete(Slist, pasCount, index);
            string showkey = x.key;
            if (!showkey.empty())
                encryptDecrypt(&showkey[0]);
            cout << "Username:" << x.Username << "/"
                << "URL:" << x.URL << "/"
                << "key:" << toHex(x.key) << "(" << showkey << ")" << "/"
                << "UID:" << x.UID << "Delete succeeded!\n";
            system("pause");
            break;
        }
        case 6: {  //[6].DeleteUserdata
            int index = 0;
            cout << "Input index : ";
            if (!(cin >> index)) {      //输入了非数字，别让 index 带着 0 往下走
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Invalid input, please enter a number!" << endl;
                system("pause");
                break;
            }
            //先定位一下：管理员在 users[0]，不允许删除
            int pos = (index > 9000) ? LocateUserByUid(index) : index;
            if (pos == 0) {
                cout << "管理员账号不允许删除！" << endl;
                system("pause");
                break;
            }

            //index > 9000 时 Delete 内部按 UID 删除，否则按位置删
            User deleted = Delete(users, userCount, index);
            if (deleted.uid == 0) {     //删除失败时返回的是值初始化的空记录
                system("pause");
                break;
            }

            cout << "\n用户删除成功，被删除的用户数据：" << endl;
            PrintLineUser(deleted); //

            //级联删除该 UID 在数据组里缓存的全部数据
            int removed = 0;
            for (int i = 0; i < pasCount; ) {
                if (Slist[i].UID == deleted.uid) {
                    PasswordItem d = Delete(Slist, pasCount, i);
                    PrintLineData(d);
                    removed++;
                    //这里不能再写 i++：删掉后 i 位置已经换成了后面的元素，i 自增会跳过一个
                }
                else {
                    i++;
                }
            }

            if (removed == 0)
                cout << "该用户在数据组中没有缓存数据。" << endl;
            else
                cout << "共删除 " << removed << " 条数据。" << endl;

            //删掉的正好是当前登录用户时，登录态一起重置
            if (userUid == deleted.uid) {
                userUid = 000;
                ruletoken = -1;
            }

            system("pause");
            break;
        }
        case 7: {  //[7].打印全部密码数据
            ListData();     //admin 走 seeAll 那一层，列出所有用户的数据
            system("pause");
            break;
        }
        case 8: {  //[8].PrintUserdata 还没写
            PrintUserData();
            system("pause");
            break;
        }
        case 9: {  //[9].Logout
            userUid = 000;      //登出重置 000
            ruletoken = -1;
            return;
        }
        default: {
            cout << "Invalid choice, please try again!" << endl;
            system("pause");
            break;
        }
        }
    }
}

void userMenu() {                                                              //普通用户菜单
    if (ruletoken != RULE_USER) {   //兜底：这个菜单只给普通用户开
        cout << "无权限访问用户菜单！" << endl;
        system("pause");
        return;
    }

    int choice;
    while (true) {

        if (ruletoken != RULE_USER) {   //兜底：防止return没有正确切出！
            cout << "无权限访问用户菜单！" << endl;
            system("pause");
            return;
        }

        system("cls");
        cout << "+================== 用户菜单 ===================+" << endl;
        cout << "| [1].查看我的账号信息                          |" << endl;
        cout << "| [2].查看我的密码数据                          |" << endl;
        cout << "| [3].插入我的密码数据                          |" << endl;
        cout << "| [4].按位置删除我的数据                        |" << endl;
        cout << "| [5].退出登录                                  |" << endl;
        cout << "| [6].注销该账号                                |" << endl;
        cout << "+-----------------------------------------------+" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        if (!cin) {
            bool atEof = cin.eof();
            cin.clear();
            cin.ignore(1000, '\n');
            if (atEof)
                return;
            cout << "Invalid input, please enter a number!" << endl;
            system("pause");
            continue;
        }

        switch (choice) {
        case 1: {  //[1].查看我的账号信息
            int pos = LocateUserByUid(userUid);     //只认全局变量里的 UID，不让输别人
            if (pos == -1)
                cout << "找不到当前登录用户！" << endl;
            else
                PrintLineUser(users[pos]);
            system("pause");
            break;
        }
        case 2: {  //[2].查看我的密码数据
            ListData();     //user 走 UID 匹配那一层，只看得到自己的
            system("pause");
            break;
        }
        case 3: {  //[3].插入我的密码数据
            PasswordItem item;
            cout << "Enter your data(name;URL;key): ";
            cin >> item.Username >> item.URL >> item.key;
            item.UID = userUid;             //UID 按全局变量自动匹配，不让用户填
            encryptDecrypt(&item.key[0]);   //写入表之前先加密
            Addend(Slist, pasCount, item, Max_Password);
            cout << "插入成功！" << endl;
            system("pause");
            break;
        }
        case 4: {  //[4].按位置删除我的数据
            int seq = 0;
            cout << "Input index (列表里的序号): ";
            if (!(cin >> seq)) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Invalid input, please enter a number!" << endl;
                system("pause");
                break;
            }
            //用户输的是"我的第几条"，不是表里的绝对下标，先还原成下标
            int index = LocateMyDataBySeq(seq);
            if (index == -1) {
                cout << "没有这条数据！" << endl;
                system("pause");
                break;
            }
            if (Slist[index].UID != userUid) {  //兜底：还原出来的必须还是自己的
                cout << "只能删除自己的数据！" << endl;
                system("pause");
                break;
            }
            PasswordItem x = Delete(Slist, pasCount, index);
            cout << "删除成功：" << endl;
            PrintLineData(x, false);        //不显示 UID
            system("pause");
            break;
        }
        case 5: {  //[5].退出登录
            userUid = 000;      //登出重置 000
            ruletoken = -1;
            return;
        }
        case 6: {  //[6].注销账号
            int index;
            index = LocateUserByUid(userUid);
            User deleted = Delete(users, userCount, index);
            if (deleted.uid == 0) {     //删除失败时返回的是值初始化的空记录
                system("pause");
                break;
            }

            cout << "\n 注销成功，被注销的用户数据：" << endl;
            PrintLineUser(deleted); //

            //级联删除该 UID 在数据组里缓存的全部数据
            int removed = 0;
            for (int i = 0; i < pasCount; ) {
                if (Slist[i].UID == deleted.uid) {
                    PasswordItem d = Delete(Slist, pasCount, i);
                    PrintLineData(d);
                    removed++;
                    //这里不能再写 i++：删掉后 i 位置已经换成了后面的元素，i 自增会跳过一个
                }
                else {
                    i++;
                }
            }

            if (removed == 0)
                cout << "该用户在数据组中没有缓存数据。" << endl;
            else
                cout << "共删除 " << removed << " 条数据。" << endl;

            //登录态一起重置
            if (userUid == deleted.uid) {
                userUid = 000;
                ruletoken = -1;
            }

            system("pause");
            return;
        }
        default: {
            cout << "Invalid choice, please try again!" << endl;
            system("pause");
            break;
        }
        }
    }
}

///加密解密
void encryptDecrypt(char* str) {                                               //对称加解密（异或加密密钥: 0x7F）
    const char key = 0x7F;
    for (int i = 0; str[i] != '\0'; i++)
    {
        str[i] = str[i] ^ key;
    }

}

//把密文按十六进制打出来。异或 0x7F 之后的字节大多落在控制字符区（pw -> 0F 08），
//因为直接 cout 一个字符都看不见，转成 hex 才能跟明文一起显示出来。
string toHex(const string& s) {                                                //密文转十六进制字符串，AI
    const char* digits = "0123456789ABCDEF";
    string out;
    for (size_t i = 0; i < s.size(); i++) {
        unsigned char c = (unsigned char)s[i];
        out += digits[c >> 4];
        out += digits[c & 0x0F];
    }
    return out;
}

int hexVal(char c) {                                                          //单个十六进制字符转成数值
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;   //非法字符当 0 处理，不抛异常
}

//toHex 的逆操作：把 hex 字符串还原成原来的字节
//文件里的 key 存的是 hex，读回来要先还原成密文，再交给 encryptDecrypt 才是明文
string fromHex(const string& s) {                                              //十六进制字符串转回密文
    string out;
    for (size_t i = 0; i + 1 < s.size(); i += 2) {
        out += (char)((hexVal(s[i]) << 4) | hexVal(s[i + 1]));
    }
    return out;
}

//文件读写
void outFile() {                                                               //将数据覆盖到文件
    ofstream outUser(USER_FILE);
    ofstream outData(DATA_FILE);

    if (!outUser.is_open() || !outData.is_open()) {
        cout << "[INFO] - File data error!\n";
        system("pause");
        return;
    }

    //分隔符用 \t：name / URL 都是 cin >> 读进来的，不可能含空白，所以不会跟分隔符撞
    //key 一律写成 hex（toHex）：密文是异或后的裸字节，里面可能正好是分隔符乃至 0x0A/0x0D（明文 u / r）
    for (int i = 0; i < userCount; ++i) {
        outUser << users[i].name << '\t' << toHex(users[i].key) << '\t' << users[i].rule << '\t' << users[i].uid << '\n';
    }

    for (int i = 0; i < pasCount; ++i) {
        outData << Slist[i].Username << '\t' << Slist[i].URL << '\t' << toHex(Slist[i].key) << '\t' << Slist[i].UID << '\n';
    }

    outUser.close();
    outData.close();
}

void inFile() {                                                                //将数据从文件读入
    ifstream inUser(USER_FILE);
    ifstream inData(DATA_FILE);

    if (!inUser.is_open() || !inData.is_open()) {
        return;
    }

    string line;    //从文件中获取用户数据
    if (inUser.is_open()) {
        userCount = 0;
        while (getline(inUser, line) && userCount < Max_User) {
            if (line.empty())continue;
            stringstream ss(line);

            string iName, iKey, iRule, iUid;    //按 outFile 写出的 name/key/rule/uid 顺序读
            getline(ss, iName, '\t');
            getline(ss, iKey, '\t');
            getline(ss, iRule, '\t');
            getline(ss, iUid, '\t');

            users[userCount].name = iName;
            users[userCount].key = fromHex(iKey);   //文件里是 hex，先还原成密文字节
            users[userCount].rule = atoi(iRule.c_str());
            users[userCount].uid = atoi(iUid.c_str());

            userCount++;
        }
        inUser.close();
    }

    if (inData.is_open()) {
        pasCount = 0;
        while (getline(inData, line) && pasCount < Max_Password) {
            if (line.empty())continue;
            stringstream ss(line);

            string iUsername, iUrl, iKey, iUid;    //按 outFile 写出的 Username/URL/key/UID 顺序读
            getline(ss, iUsername, '\t');
            getline(ss, iUrl, '\t');
            getline(ss, iKey, '\t');
            getline(ss, iUid, '\t');

            Slist[pasCount].Username = iUsername;
            Slist[pasCount].URL = iUrl;
            Slist[pasCount].key = fromHex(iKey);    //文件里是 hex，先还原成密文字节
            Slist[pasCount].UID = atoi(iUid.c_str());

            pasCount++;
        }
        inData.close();
    }

}