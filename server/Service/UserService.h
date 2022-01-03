//
// Created by cswen on 2020/8/3.
//

#pragma once
#include"../Dao/MySQLConnector.h"

using namespace std;

class UserService {
private:
    MySQLConnector *connector;

    bool checkAccount(string &account);

    bool checkPassword(string &password);

public:
    UserService();

    pair<int, string> checkLogin(string account, string password);

    bool isRegistered(string account);

    bool registerUser(string account, string username, string password);

    vector<string> CheckTaskOrder(const string id);

    bool isTaskOrderRegister(const string id);

    bool registerOrder(const string& id, const string& fw, const string& fwVersion, int cnt, const string& creater);

    bool updateOrderCnt(const string& id, int cnt);

    bool deleteOrder(const string& id);

    vector<string> CheckFirmWare(const string& name, const string& version);

    bool isFirmWareRegister(const string& name, const string& version);

    bool registerFirmWare(const string& name, const string& version, const string& creater);

    bool deleteFirmWare(const string& name, const string& version);
};

