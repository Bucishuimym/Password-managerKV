#include <cstring>
#include <iostream>
#include <string>
#include <cstdlib>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <conio.h>

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
    cout << "Respect the privacy of others." << endl;
    cout << "Think before you type." << endl;
    cout << "With great power comes great responsibility." << endl;
}

std::string getInput(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
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

void PrintLineUser(UserGroup& usr) {
    cout <<"Name:" << usr.GetName() <<"ID" << usr.GetUid() << endl;
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

    if (!users.PushFront(newUser)) {
        if (!users.PushBack(newUser)) {
            std::cerr << "Failed to add user to list." << std::endl;
            system("pause");
            return;
        }
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
        cout << "| [9].退出登录     799                             |" << endl;
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
                DoubleList<pwDataGroup>::Node* p;
                 p=pwdata.LocateByUid(id);
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
        case 3: { // [3]. InsertData
            int n;
            cout << "Input number of pos: ";
            cin >> n;
            cin.ignore();   // 吃掉残留换行

            pwDataGroup item;
            cout << "Enter your data(name;URL;key): ";
            item.input();

            item.SetUid(userUid);
            encryptDecrypt(item.GetKey().data());
            Insert(n,item);
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
            DoubleList<pwDataGroup>::Node* p;
            pwDataGroup x = Delete(p, pasCount, index);
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
            //先定位一下：管理员在 users[0]，不允许删除
            int pos = (index > 9000) ? LocateUserByUid(index) : index;
            if (pos == 0) {
                cout << "管理员账号不允许删除！" << endl;
                system("pause");
                break;
            }

            //index > 9000 时 Delete 内部按 UID 删除，否则按位置删
            pwDataGroup deleted = DeleteByUid(users, index);
            if (deleted.GetUid() == 0) {     //删除失败时返回的是值初始化的空记录
                system("pause");
                break;
            }

            cout << "\n用户删除成功，被删除的用户数据：" << endl;
            PrintLineUser(deleted); //

            //级联删除该 UID 在数据组里缓存的全部数据
            int removed = 0;
            for (int i = 0; i < pasCount; ) {
                DoubleList<pwDataGroup>::Node* p;
                if (p->data.GetUid() == deleted.GetUid()) {
                    pwDataGroup d = Delete(p, pasCount, i);
                    PrintLineData(d);
                    removed++;
                    //这里不能再写 i++：删掉后 i 位置已经换成了后面的元素，i 自增会跳过一个
                }
                else {
                    p = p->next;
                }
            }

            if (removed == 0)
                cout << "该用户在数据组中没有缓存数据。" << endl;
            else
                cout << "共删除 " << removed << " 条数据。" << endl;

            //删掉的正好是当前登录用户时，登录态一起重置
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
            pwDataGroup item;
            cout << "Enter your data(name;URL;key): ";
            item.inPut();
            item.SetUid(userUid);             //UID 按全局变量自动匹配，不让用户填
            encryptDecrypt(item.GetKey().data());   //写入表之前先加密
            DoubleList<pwDataGroup>::Node* p;
            Addend(p, pasCount, item, Max_Password);
            cout << "插入成功！" << endl;
            system("pause");
            break;
        }
        case 4: {  //[4].按位置删除我的数据
            int seq = 0;
            cout << "Input index (列表里的序号): ";
            DoubleList<pwDataGroup>::Node* p;
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
            if (p->data.GetUid() != userUid) {  //兜底：还原出来的必须还是自己的
                cout << "只能删除自己的数据！" << endl;
                system("pause");
                break;
            }
            pwDataGroup x = Delete(p, pasCount, index);
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
            pwDataGroup deleted = Delete(users, userCount, index);
            if (deleted.GetUid() == 0) {     //删除失败时返回的是值初始化的空记录
                system("pause");
                break;
            }

            cout << "\n 注销成功，被注销的用户数据：" << endl;
            PrintLineUser(deleted); //

            //级联删除该 UID 在数据组里缓存的全部数据
            int removed = 0;
            for (int i = 0; i < pasCount; ) {
                DoubleList<pwDataGroup>::Node* p;
                if (p->data.GetUid() == deleted.GetUid()) {
                    pwDataGroup d = Delete(p, pasCount, i);
                    PrintLineData(d);
                    removed++;
                    //这里不能再写 i++：删掉后 i 位置已经换成了后面的元素，i 自增会跳过一个
                }
                else {
                    p = p->next;
                }
            }

            if (removed == 0)
                cout << "该用户在数据组中没有缓存数据。" << endl;
            else
                cout << "共删除 " << removed << " 条数据。" << endl;

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
    DoubleList<UserGroup>::Node* p;
    //分隔符用 \t：name / URL 都是 cin >> 读进来的，不可能含空白，所以不会跟分隔符撞
    //key 一律写成 hex（toHex）：密文是异或后的裸字节，里面可能正好是分隔符乃至 0x0A/0x0D（明文 u / r）
    for (int i = 0; i < userCount; ++i) {
        outUser << p->data.GetName() << '\t' << toHex(p->data.GetKey()) << '\t' << p->data.GetRule()<< '\t' << p->data.GetUid() << '\n';
        p = p->next;
    }
    DoubleList<pwDataGroup>::Node* q;
    for (int i = 0; i < pasCount; ++i) {
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
            DoubleList<pwDataGroup>::Node* p;
            if (line.empty())continue;
            stringstream ss(line);

            string iUsername, iUrl, iKey, iUid;    //按 outFile 写出的 Username/URL/key/UID 顺序读
            getline(ss, iUsername, '\t');
            getline(ss, iUrl, '\t');
            getline(ss, iKey, '\t');
            getline(ss, iUid, '\t');

            p->data.GetUsername() = iUsername;
            p->data.GetUrl() = iUrl;
            p->data.GetKey() = fromHex(iKey);    //文件里是 hex，先还原成密文字节
            p->data.GetUid() = atoi(iUid.c_str());
            p = p->next;
            pasCount++;
        }
        inData.close();
    }

}