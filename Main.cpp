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
#pragma comment(lib,"DoubleLinkedList.lib")

using namespace std;

int main() {

    inFile();                       //先尝试从文件恢复
    if (userCount == 0)             //users.txt 不存在或压根没读到用户，才算首次运行
        initAdmin();                //只有首次运行才建 admin，否则每次启动都会多出来一个

    int choice;
    while (true)
    {
        system("cls");
        cout << "=================================================================" << endl;
        cout << "                   +----- Main Menu -----+" << endl;
        cout << "                   |  1. Login           |" << endl;
        cout << "                   |  2. Sign Up         |" << endl;
        cout << "                   |  3. Logout          |" << endl;
        cout << "                   +---------------------+" << endl;
        cout << endl;
        cout << "Enter your choice please: ";
        cin >> choice;
        if (!cin) {     //跟子菜单一样兜一道：没有这个 guard，非数字输入会让 failbit 钉死、菜单无限刷屏
            bool atEof = cin.eof();     //要在 clear() 之前判断，clear() 会把 eofbit 一起清掉
            cin.clear();
            cin.ignore(1000, '\n');
            if (atEof) {
                outFile();              //输入流到头了
                return 0;
            }
            cout << "Invalid input, please enter a number!" << endl;
            system("pause");
            continue;
        }

        switch (choice) {
        case 1:
        {
            int loginToken;
            while (true)
            {
                loginToken = login(ruletoken);
                if (loginToken == 1)
                {
                    if (ruletoken == RULE_ADMIN) {
                        adminMenu();
                        outFile();      
                    }
                    else if (ruletoken == RULE_USER) {
                        userMenu();
                        outFile();
                    }
                    else
                        throw "[INFO] - error: Unknown rule token";
                    break;
                }
                else if (loginToken == -1)
                    break;
            }
            break;
        }
        case 2:
            signUp();
            break;
        case 3:
            outFile();                  //退出前把内存里的两张表覆盖回文件
            cout << "Thank you again!" << endl;
            system("pause");
            return 0;
            break;
        default:
        {
            cout << "Invalid choice, please try again!" << endl;
            system("pause");
            break;
        }
        }
    }


    return 0;
}