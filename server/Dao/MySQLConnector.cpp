//
// Created by cswen on 2020/8/3.
//

#include "MySQLConnector.h"

void MySQLConnector::init() {
    conn = mysql_init(NULL);
    conn = mysql_real_connect(conn, HOST, USERNAME, PASSWORD, DATABASE, PORT, NULL, 0);
    if (!conn) {
        cout << "mysql_real_connect fail" << endl;
        exit(-1);
    }
    mysql_query(conn, "set names utf8");
}

MySQLConnector *MySQLConnector::getMySQLConnector() {
    if (connector == nullptr) {
        connector = new MySQLConnector;
        connector->init();
    }
    return connector;
}

pair<int, string> MySQLConnector::queryUser(const string& account, const string& password) {
    string querySql =
            "select account, username from user where account = " + account + " and password = " + "\"" + password +
            "\"";
    pair<int, string> user;
    int res = mysql_query(conn, querySql.data());

    if (!res) {
        MYSQL_RES *result = mysql_store_result(conn);
        if (result && mysql_num_rows(result)) {
            MYSQL_ROW row = mysql_fetch_row(result);
            user.first = atoi(row[0]);
            user.second = row[1];
        }
        mysql_free_result(result);
    }
    return user;
}

bool MySQLConnector::queryUser(const string& account) {
    string querySql = "select account from user where account = " + account;
    int res = mysql_query(conn, querySql.data());
    bool flag = false;
    if (!res) {
        MYSQL_RES *result = mysql_store_result(conn);
        if (result && mysql_num_rows(result)) {
            flag = true;
        }
        mysql_free_result(result);
    }
    return flag;
}

bool MySQLConnector::insertUser(const string& account, const string& username, const string& password) {
    string insertSql = "insert into `user` values(" +
                       account + ","
                                 "\"" + username + "\","
                       + "\"" + password + "\","
                       + "\"" + MyTime::getCurrentFormatTimeStr() + "\")";
    int res = mysql_query(conn, insertSql.data());
    return res == 0;
}

MySQLConnector::~MySQLConnector() {
    mysql_close(conn);
    delete connector;
}

vector<string> MySQLConnector::queryOrder(const string& id) {
    string querySql = "select * from taskorder where id = " "\'" + id + "\'";
    std::cout << "querySql:" << querySql << std::endl;
    int res = mysql_query(conn, querySql.data());
    vector<string> orderdata;
    if (!res) {
        MYSQL_RES *result = mysql_store_result(conn);
        if (result && mysql_num_rows(result)) {
            MYSQL_ROW row = mysql_fetch_row(result);
            for (int i = 0; i < 6; i++) {
                orderdata.push_back(row[i]);
            }
        }
        mysql_free_result(result);
    }
    return orderdata;
}

// update taskorder set cnt = 99, creater = 'sss' where id = 'EMS00001';
bool MySQLConnector::updateOrderCnt(const string& id, int cnt) {
    // INSERT INTO taskorder(id, fw, fwversion, cnt, creater)  VALUES('EMS00002', 'Boost.hex', '0.1', 100, '2021:12:28', 'wuhen');
    string insertSql = "update taskorder set cnt = " + to_string(cnt) + ","
                       " createtime = " "\'" + MyTime::getCurrentFormatTimeStr() + "\'"
                       " where id = " "\'" + id + "\'";
    cout << "insertSql:" << insertSql << endl;
    int res = mysql_query(conn, insertSql.data());
    return res == 0;
}

bool MySQLConnector::insertOrder(const string& id, const string& fw, const string& fwVersion, int& cnt, 
    const string& creater) {
    // INSERT INTO taskorder(id, fw, fwversion, cnt, creater)  VALUES('EMS00002', 'Boost.hex', '0.1', 100, '2021:12:28', 'wuhen');
    string insertSql = "insert into `taskorder` values("
                       "\'" + id + "\'" ","
                       "\'" + fw + "\'" ","
                       "\'" + fwVersion + "\'" ","
                       + to_string(cnt) + ","
                       "\'" + MyTime::getCurrentFormatTimeStr() + "\'" ","
                       "\'" + creater + "\'" ")";
    cout << "insertSql:" << insertSql << endl;
    int res = mysql_query(conn, insertSql.data());
    return res == 0;
}

bool MySQLConnector::deleteOrder(const string& id) {
    string querySql = "delete from taskorder where id = " "\'" + id + "\'";
    std::cout << "querySql:" << querySql << std::endl;
    int res = mysql_query(conn, querySql.data());
    if (!res) {
        return true;
    }
    return false;
}

vector<string> MySQLConnector::queryFirmWare(const string& name, const string& version) {
    string querySql = "select * from firmware where name = " "\'" + name + "\'" " and " "version = " "\'" + version + "\'";
    int res = mysql_query(conn, querySql.data());
    vector<string> firmware;
    if (!res) {
        MYSQL_RES *result = mysql_store_result(conn);
        if (result && mysql_num_rows(result)) {
            MYSQL_ROW row = mysql_fetch_row(result);
            for (int i = 0; i < 4; i++) {
                firmware.push_back(row[i]);
            }
        }
        mysql_free_result(result);
    }
    return firmware;
}

bool MySQLConnector::insertFirmWare(const string& name, const string& version, const string& creater) {
    string insertSql = "insert into `firmware` values("
                       "\'" + name + "\'" ","
                       "\'" + version + "\'" ","
                       "\'" + MyTime::getCurrentFormatTimeStr() + "\'" ","
                       "\'" + creater + "\'" ")";
    int res = mysql_query(conn, insertSql.data());
    return res == 0;
}

bool MySQLConnector::deleteFirmWare(const string& name, const string& version) {
    string querySql = "delete from firmware where name = " "\'" + name + "\'" " and " "version = " "\'" + version + "\'";
    std::cout << "querySql:" << querySql << std::endl;
    int res = mysql_query(conn, querySql.data());
    if (!res) {
        return true;
    }
    return false;
}

MySQLConnector *MySQLConnector::connector = nullptr;
