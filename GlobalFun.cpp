#include <cstring>
#include <iostream>
#include <string>
#include <cstdlib>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <conio.h>
#include <io.h>     //_isatty
#include <cstdio>   //_fileno(stdin)

#include "pwDataGroup.h"
#include "UserGroup.h"
#include "OverAllkv.h"
#include "DLinkedList.h"
#pragma comment(lib,"DoubleLinkedList.lib")

using namespace std;

void initAdmin() {                                                             //初始化管理员账号
    UserGroup admin;
    std::string key = "123";
    if (!key.empty()) encryptDecrypt(&key[0]);    // 对字符串就地加密

    admin.SetName("admin");
    admin.SetKey(key);
    admin.SetRule(RULE_ADMIN);
    admin.SetUid(uidCount());

    // 插入表头，失败则尾插兜底
    if (!users.PushFront(admin)) {
        users.PushBack(admin);
    }

    userCount = 1;
}

int uidCount() {                                                               //生成下一个用户 UID
    int usC = userCount;
    int num = usC++;
    int UID = (num + 234565) % 1000 + 9000;
    return UID;
}

void LinuxLecture() {
    cout << endl;
    cout << "       (1)Respect the privacy of others." << endl;
    cout << "       (2)Think before you type." << endl;
    cout << "       (3)With great power comes great responsibility." << endl;
}

std::string getInput(const std::string& prompt) {
    std::cout << prompt;
    std::string input;

    
    if (!_isatty(_fileno(stdin))) {
        std::getline(std::cin >> std::ws, input);
        return input;
    }

    int ch;
    while (true) {
        ch = _getch();
        if (ch == '\r' || ch == '\n') {
            std::cout << std::endl;
            break;
        }
        if (ch == 8) { // backspace
            if (!input.empty()) {
                input.pop_back();
                std::cout << "\b \b";
            }
        }
        else if (ch >= 32 && ch <= 126) {
            input.push_back((char)ch);
            std::cout << '*';
        }
    }
    return input;
}

void PrintLineUser(const UserGroup& usr) {
    cout <<"Name:" << usr.GetName() <<" ID:" << usr.GetUid() << endl;
    if (usr.GetRule() == RULE_ADMIN) {
        cout << "Administrator account." << endl;
    }
}

int login(int& outRule) {                                                      //登录，成功写回权限令牌
    userUid = 000;
    string inputName, inputKey;
    system("cls");
    cout << "============== User Login ==============" << endl;

    cout << "Enter user name: "; cin >> inputName;
    cout << "Enter user key: ";  cin >> inputKey;

    string key = inputKey;
    if (!key.empty()) encryptDecrypt(&key[0]);

    auto p = users.FindIf([&](const UserGroup& u) { return u.GetName() == inputName; });
    if (!p) {
        cout << "\nUsername no found!" << endl;
        system("pause");
        return -1;
    }
    if (p->data.GetKey() != key) {
        cout << "\nPassword error!" << endl;
        system("pause");
        return -1;
    }

    cout << "\nLogin Successful Hello," << p->data.GetName() << "\n" << endl;
    outRule = p->data.GetRule();
    userUid = p->data.GetUid();
    PrintLineUser(p->data);
    system("pause");

    if (p->data.GetRule() == RULE_ADMIN) {
        system("cls");
        LinuxLecture();
        system("pause");
        system("cls");
    }

    return 1;
}

void signUp() {                                                                //注册
    system("cls");
    if (userCount >= Max_User) {
        std::cout << "Sorry! user max" << std::endl;
        system("pause");
        return;
    }

    std::string name;
    std::cout << "============== Sign up ==============" << std::endl;
    std::cout << "Enter new user name: ";
    std::cin >> name;

    if (users.FindIf([&](const UserGroup& u) { return u.GetName() == name; })) {
        std::cout << "Username already exisit！" << std::endl;
        system("pause");
        return;
    }

    std::string key = getInput("Enter new user key: ");
    std::string k = getInput("Again: ");
    if (key != k) {
        std::cout << "Passwerd error! try again." << std::endl;
        system("pause");
        return;
    }

    if (!key.empty()) encryptDecrypt(&key[0]);

    UserGroup newUser;
    newUser.SetName(name);
    newUser.SetKey(key);
    newUser.SetRule(RULE_USER);
    newUser.SetUid(uidCount());

    
    if (!users.PushBack(newUser)) {
        std::cerr << "Failed to add user to list." << std::endl;
        system("pause");
        return;
    }

    userCount++;
    std::cout << "Registration succeeded!" << std::endl;
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
        cin >> choice;      
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
                auto* p = pwdata.LocateByUid(id);   //按 UID 直接拿到结点
                string showkey = p->data.GetKey();
                if (!showkey.empty())
                    encryptDecrypt(&showkey[0]);
                cout << "|————" << p->data.GetUsername()<< "\n"
                    << "  |——" << p->data.GetUrl() << "\n"
                    << "  |——key:" << toHex(p->data.GetKey()) << "(" << showkey << ")" << "\n"
                    << "  |_" << p->data.GetUid() << endl
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
                UserGroup u;
                users.Get(pos + 1, u);      //定位给的是下标，链表的位序从 1 开始
                string showkey = u.GetKey();
                if (!showkey.empty())
                    encryptDecrypt(&showkey[0]);
                cout << "|————" << u.GetName() << "\n"
                    << "  |——key:" << toHex(u.GetKey()) << "(" << showkey << ")" << endl
                    << "  |_" << u.GetUid() << endl
                    << endl;
            }
            system("pause");
            break;
        }
        case 3: { // [3]. InsertData
            int n;
            cout << "Input number of pos: ";
            cin >> n;
            cin.ignore();   // 吃掉残留换行

            pwDataGroup item;
            cout << "Enter your data(name;URL;key): ";
            item.inPut();

            item.SetUid(userUid);               //UID 按当前登录用户自动匹配
            string key = item.GetKey();         //GetKey 返回的是拷贝，加密完要写回去
            if (!key.empty())
                encryptDecrypt(&key[0]);
            item.SetKey(key);

            
            if (pwdata.Insert(n + 1, item))
                pasCount++;
            else
                cout << "Insertion failed: Invalid position or data table is full!" << endl;
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
            pwDataGroup x;
         
            bool ok = (index > 9000) ? pwdata.DeleteByUid(index, x)
                : pwdata.DeleteByPos(index + 1, x);
            if (!ok) {
                cout << "Deletion failed: Invalid index/UID!" << endl;
                system("pause");
                break;
            }
            pasCount--;
            string showkey = x.GetKey();
            if (!showkey.empty())
                encryptDecrypt(&showkey[0]);
            cout << "Username:" << x.GetUsername() << "/"
                << "URL:" << x.GetUrl()<< "/"
                << "key:" << toHex(x.GetKey()) << "(" << showkey << ")" << "/"
                << "UID:" << x.GetUid() << "Delete succeeded!\n";
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
            
            int pos = (index > 9000) ? LocateUserByUid(index) : index;
            if (pos == 0) {
                cout << "The administrator account cannot be deleted!" << endl;
                system("pause");
                break;
            }

            
            UserGroup deleted;
            bool ok = (index > 9000) ? users.DeleteByUid(index, deleted)
                : users.DeleteByPos(index + 1, deleted);
            if (!ok) {
                cout << "User to be deleted not found!" << endl;
                system("pause");
                break;
            }
            userCount--;

            cout << "\nUser deletion successful. Deleted user data:" << endl;
            PrintLineUser(deleted); //

            
            int removed = 0;
            pwDataGroup d;
            while (pwdata.DeleteByUid(deleted.GetUid(), d)) {
                PrintLineData(d);
                removed++;
            }
            pasCount -= removed;

            if (removed == 0)
                cout << "No cached data exists for this user in the data group." << endl;
            else
                cout << "Total deleted" << removed << " pieces of data." << endl;

            
            if (userUid == deleted.GetUid()) {
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
    if (ruletoken != RULE_USER) {  
        cout << "No permission to access the user menu!" << endl;
        system("pause");
        return;
    }

    int choice;
    while (true) {

        if (ruletoken != RULE_USER) {   
            cout << "No permission to access the user menu!" << endl;
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
                cout << "Current logged-in user not found!" << endl;
            else {
                UserGroup u;
                users.Get(pos + 1, u);  //定位给的是下标，链表的位序从 1 开始
                PrintLineUser(u);
            }
            system("pause");
            break;
        }
        case 2: {  //[2].查看我的密码数据
            ListData();     //user 走 UID 匹配那一层，只看得到自己的
            system("pause");
            break;
        }
        case 3: {  //[3].插入我的密码数据
            pwDataGroup item;
            cout << "Enter your data(name;URL;key): ";
            item.inPut();
            item.SetUid(userUid);             //UID 按全局变量自动匹配，不让用户填
            string key = item.GetKey();       //GetKey 返回的是拷贝，加密完要写回去
            if (!key.empty())
                encryptDecrypt(&key[0]);      //写入表之前先加密
            item.SetKey(key);
            if (pwdata.PushBack(item)) {      //尾插，表满了 PushBack 返回 false
                pasCount++;
                cout << "Inserted successfully!" << endl;
            }
            else
                cout << "Insertion failed: The data table is full!" << endl;
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
            
            int index = LocateMyDataBySeq(seq);
            if (index == -1) {
                cout << "No such data!" << endl;
                system("pause");
                break;
            }
            pwDataGroup mine;
            if (!pwdata.Get(index + 1, mine) || mine.GetUid() != userUid) {  //兜底：还原出来的必须还是自己的
                cout << "Only your own data can be deleted!" << endl;
                system("pause");
                break;
            }
            pwDataGroup x;
            if (!pwdata.DeleteByPos(index + 1, x)) {    //位序上面刚验过，删得掉
                system("pause");
                break;
            }
            pasCount--;
            cout << "Data deleted successfully:" << endl;
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
            
            UserGroup deleted;
            if (!users.DeleteByUid(userUid, deleted)) {
                system("pause");
                break;
            }
            userCount--;

            cout << "\n Cancellation successful, cancelled user data：" << endl;
            PrintLineUser(deleted); //

            
            int removed = 0;
            pwDataGroup d;
            while (pwdata.DeleteByUid(deleted.GetUid(), d)) {
                PrintLineData(d);
                removed++;
            }
            pasCount -= removed;

            if (removed == 0)
                cout << "No cached data exists for this user in the data group." << endl;
            else
                cout << "Total deleted " << removed << " pieces of data." << endl;

            //登录态一起重置
            if (userUid == deleted.GetUid()) {
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
    
    for (auto* p = users.Begin(); !users.AtEnd(p); p = users.Next(p)) {
        outUser << p->data.GetName() << '\t' << toHex(p->data.GetKey()) << '\t' << p->data.GetRule()<< '\t' << p->data.GetUid() << '\n';
    }
    for (auto* q = pwdata.Begin(); !pwdata.AtEnd(q); q = pwdata.Next(q)) {
        outData << q->data.GetUsername() << '\t' << q->data.GetUrl() << '\t' << toHex(q->data.GetKey()) << '\t' << q->data.GetUid() << '\n';
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
        users.Clear();                  
        userCount = 0;
        while (getline(inUser, line)) {
            if (line.empty())continue;
            stringstream ss(line);

            string iName, iKey, iRule, iUid;   
            getline(ss, iName, '\t');
            getline(ss, iKey, '\t');
            getline(ss, iRule, '\t');
            getline(ss, iUid, '\t');

            UserGroup u;
            u.SetName(iName);
            u.SetKey(fromHex(iKey));    //文件里是 hex，先还原成密文字节
            u.SetRule(atoi(iRule.c_str()));
            u.SetUid(atoi(iUid.c_str()));

            if (!users.PushBack(u))
                break;                  
            userCount++;
        }
        inUser.close();
    }

    if (inData.is_open()) {
        pwdata.Clear();                 //同用户表，重读之前先清空
        pasCount = 0;
        while (getline(inData, line)) {
            if (line.empty())continue;
            stringstream ss(line);

            string iUsername, iUrl, iKey, iUid;    
            getline(ss, iUsername, '\t');
            getline(ss, iUrl, '\t');
            getline(ss, iKey, '\t');
            getline(ss, iUid, '\t');

            pwDataGroup d;
            d.SetUsername(iUsername);
            d.SetUrl(iUrl);
            d.SetKey(fromHex(iKey));    //文件里是 hex，先还原成密文字节
            d.SetUid(atoi(iUid.c_str()));

            if (!pwdata.PushBack(d))
                break;                  
            pasCount++;
        }
        inData.close();
    }

}