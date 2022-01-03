#include<iostream>
#include<string>
#include <fcntl.h>
#include <sys/stat.h>
#include<vector>
#include<algorithm>
#include<arpa/inet.h>
#include<cstdlib>
#include<unistd.h>
#include<sys/epoll.h>
#include<json/json.h>
#include"ProtocolHead/HeadData.h"
#include"Service/DataProcesser.h"
#include "Service/UserService.h"
#include "Service/Online.h"
#include "config/server_config.h"
#include "crypto/rsacrypto.h"
#include "crypto/aescrypto.h"
#include "crypto/sha256.h"

using namespace std;

int main() {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{}, clientAddr{};
    int opt = 1;
    if (-1 == setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        cout << "setsockopt fail" << endl;
        exit(-1);
    }//设置端口复用
    int epfd = epoll_create(MAX_CONNECTIONS);
    epoll_event ev{}, events[MAX_CONNECTIONS];
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev)) {
        cout << "epoll_ctl fail" << endl;
        exit(-1);
    }
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, HOST, &serverAddr.sin_addr);
    if (-1 == bind(lfd, (sockaddr *) &serverAddr, sizeof(serverAddr))) {
        cout << "bind fail" << endl;
        exit(-1);
    }
    if (-1 == listen(lfd, MAX_CONNECTIONS)) {
        cout << "listen fail" << endl;
        exit(-1);
    }
    cout << "listening..." << endl;
    char ipAddress[BUFSIZ];
    UserService us;
    Online online;
    AesCrypto aes;
    RsaCrypto rsa;
    Sha256 sha;
    while (true) {
        int nready = epoll_wait(epfd, events, MAX_CONNECTIONS, -1);
        if (nready < 0) {
            cout << "epoll_wait error" << endl;
            exit(-1);
        }
        cout << "收到" << nready << "个请求" << endl;
        for (int i = 0; i < nready; i++) {
            int fd = events[i].data.fd;
            if (fd == lfd) {
                socklen_t len = sizeof(clientAddr);
                int cfd = accept(lfd, (sockaddr *) &clientAddr, &len);
                ev.data.fd = cfd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(clientAddr));
                //设置超时read
                struct timeval timeout = {1, 0};
                setsockopt(cfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(struct timeval));
            } else if (events[i].events & EPOLLIN) {
                HeadData hd(fd);
                unsigned int protocolId = hd.getProtocolId();
                unsigned int account = hd.getAccount();
                unsigned int dataType = hd.getDataType();
                unsigned int dataLength = hd.getDataLength();
                cout << "protocolId:" << protocolId << " " << "account:" << account << " " << "dataType:" << " " << 
                dataType << " " << "dataLength:" << dataLength << endl;
                DataProcesser dp;
                switch (protocolId) {
                    case LOGIN: {
                        cout << "login.." << endl;
                        string loginMsg = dp.readTextContent(fd, dataLength);
                        Json::Reader jsonReader;
                        Json::Value msg;
                        jsonReader.parse(loginMsg, msg);
                        string account = msg["account"].asString();
                        account = rsa.private_decrypt(account);
                        string password = msg["password"].asString();
                        password = rsa.private_decrypt(password);
                        password = sha.sha256(password);
                        string aesKey = msg["aeskey"].asString();
                        aesKey = rsa.private_decrypt(aesKey);
                        aes.password = aesKey;

                        cout << "account:" << account << endl;
                        cout << "password:" << password << endl;
                        cout << "aeskey:" << aesKey << endl;
                        pair<int, string> user = us.checkLogin(account, password);
                        Json::Value loginResult;
                        //登录成功
                        if (user.first != 0) {
                            if (online.isLogin(user.first)) {
                                loginResult["status"] = LOGIN_EXIST;
                                 cout << "user exist" << endl;
                            } else {
                                online.appendUser(user);
                                online.appendWriteFd(user.first, fd);
                                loginResult["status"] = LOGIN_SUCCESS;
                                loginResult["username"] = user.second;
                                cout << "login succeed" << endl;
                            }
                        }
                        //失败
                        else {
                            loginResult["status"] = LOGIN_FAIL;
                            cout << "login fail" << endl;
                        }
                        string loginResultStr = loginResult.toStyledString();
                        dp.writeMsg(fd, 0, loginResultStr);
                    }
                        break;
                    case REGISTER: {
                        cout << "register.." << endl;
                        string registerMsg = dp.readTextContent(fd, dataLength);
                        Json::Reader jsonReader;
                        Json::Value registerResult;
                        Json::Value msg;
                        jsonReader.parse(registerMsg, msg);
                        string account = msg["account"].asString();
                        string username = msg["username"].asString();
                        string password = msg["password"].asString();

                        cout << "account:" << account << endl;
                        cout << "username:" << username << endl;
                        cout << "password:" << password << endl;

                        if (us.isRegistered(account) || !us.registerUser(account, username, password)) {
                            registerResult["status"] = REGISTER_FAIL;
                        } else {
                            registerResult["status"] = REGISTER_SUCCESS;
                        }
                        dp.writeMsg(fd, 0, registerResult.toStyledString(), REGISTER);
                    }
                        break;

                    case SEND: {
                        cout << "send.." << endl;
                        string baseMsg = online.getUserName(account) + "(" + to_string(account) + ")说:";

                        cout << "account:" << account << endl;
                        cout << "username:" << online.getUserName(account) << endl;
                        cout << "baseMsg:" << baseMsg << endl;
                        if (dataType == TEXT) {
                            dp.writeTextToAllUser(online.getAllReadFd(), account, baseMsg);
                            string content = dp.readTextContent(fd, dataLength);
                            cout << "content:" << content << endl;
                            dp.writeTextToAllUser(online.getAllReadFd(), account, content);
                        } else if (dataType == IMAGE) {
                            string imagePath = dp.readImageContent(fd, dataLength);
                            cout << "imagePath:" << imagePath << endl;
                            if (dp.getFileLength(imagePath) == dataLength) {
                                dp.writeTextToAllUser(online.getAllReadFd(), account, baseMsg);
                                dp.writeImageToAllUser(online.getAllReadFd(), account, imagePath);
                            } else {
                                ev.data.fd = fd;
                                ev.events = EPOLLIN;
                                epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
                                close(fd);
                                close(online.getReadFd(fd));
                                string logoutMsg =
                                        online.getUserName(account) + "(" + to_string(account) + ")" + "离开了聊天室!";
                                online.removeUser(account);
                                vector<int> fds = online.getAllReadFd();
                                if (!fds.empty()) {
                                    dp.writeTextToAllUser(fds, account, logoutMsg, NOTICE);
                                    dp.writeTextToAllUser(fds, 0, online.getOnlineListStr(), ONLINELIST);
                                }
                            }
                        }
                    }
                        break;
                    case READ: {
                        cout << "read.." << endl;
                        ev.data.fd = fd;
                        ev.events = EPOLLIN;
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
                        online.appendReadFd(account, fd);
                        string loginMsg = online.getUserName(account) + "(" + to_string(account) + ")" + "走进了聊天室!";

                        cout << "account:" << account << endl;
                        cout << "username:" << online.getUserName(account) << endl;
                        cout << "loginMsg:" << loginMsg << endl;
                        dp.writeTextToAllUser(online.getAllReadFd(), account, loginMsg, NOTICE);
                        dp.writeTextToAllUser(online.getAllReadFd(), account, online.getOnlineListStr(), ONLINELIST);
                    }
                        break;
                    case LOGOUT: {
                        cout << "logout.." << endl;
                        sleep(1);
                        ev.data.fd = fd;
                        ev.events = EPOLLIN;
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
                        close(fd);
                        close(online.getReadFd(fd));
                        string logoutMsg = online.getUserName(account) + "(" + to_string(account) + ")" + "离开了聊天室!";

                        cout << "account:" << account << endl;
                        cout << "username:" << online.getUserName(account) << endl;
                        cout << "logoutMsg:" << logoutMsg << endl;
                        online.removeUser(account);
                        vector<int> fds = online.getAllReadFd();
                        cout << "当前在线人数:" << fds.size() << endl;
                        if (!fds.empty()) {
                            dp.writeTextToAllUser(fds, account, logoutMsg, NOTICE);
                            dp.writeTextToAllUser(fds, 0, online.getOnlineListStr(), ONLINELIST);
                        }
                    }
                        break;
                    case CLOSE: {
                        cout << "close.." << endl;
                        sleep(1);
                        ev.data.fd = fd;
                        ev.events = EPOLLIN;
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
                        close(fd);
                    }
                        break;
                    case GET_FW: {
                        Json::Value loginResult;
                        
                        string loginResultStr;
                        cout << "GET_FW..." << endl;
                        // 1.获取任务令
                        string baseMsg = online.getUserName(account) + "(" + to_string(account) + ")获取任务令:";

                        cout << "account:" << account << endl;
                        cout << "username:" << online.getUserName(account) << endl;
                        cout << "baseMsg:" << baseMsg << endl;

                        string order = dp.readTextContent(fd, dataLength); // 客户端发送的任务令
                        cout << "order:" << order << endl;
                        
                        string taskOrderName("./fw/");
                        string fw;
                        if (us.isTaskOrderRegister(order)) {
                            taskOrderName += order;
                            cout << "taskOrderName:" << taskOrderName << endl;
                        } else {
                            cout << "order not exist." << endl;
                            // loginResult["fw"] = "order not exist";
                            fw = "order not exist";
                            loginResult["size"] = "-1";
                            loginResultStr = loginResult.toStyledString();
                            dp.writeMsg(fd, 0, loginResultStr, SEND_FW);
                            break;
                        }

                        // 2.获取对应固件路径
                        taskOrderName +=  ".hex";
                        cout << "taskOrderName:" << taskOrderName << endl;

                        struct stat file_stat;
                        char buf[409600] = {0};
                        int n = 0;
                        int ret;
                        stat(taskOrderName.c_str(), &file_stat);
                        cout << "file size:" << file_stat.st_size << endl;

                        // 3.打开固件
                        int fwfd = open(taskOrderName.c_str(), O_RDONLY);
                        if (fwfd == -1) {
                            cout << "open hex fail." << endl;
                            // loginResult["fw"] = "open hex fail";
                            fw = "open hex fail";
                            loginResult["size"] = "-1";
                            loginResultStr = loginResult.toStyledString();
                            dp.writeMsg(fd, 0, loginResultStr, SEND_FW);
                            break;
                        }

                        // 4.读取固件内容
                        while ((n = read(fwfd, buf, sizeof(buf))) > 0) {
                            ;
                        }
                        close(fwfd);

                        // 5.发送固件
                        // string fw(std::to_string(file_stat.st_size) + "\n" + buf);
                        // dp.writeTextToAllUser(online.getAllReadFd(), account, fw, SEND_FW);

                        fw = buf;
                        // string fw(buf);
                        loginResult["fw"] = aes.aes_256_cbc_encode(fw);
                        loginResult["size"] = file_stat.st_size;
                        // cout << "loginResult[\"fw\"]:" << fw << endl;
                        cout << "size:" << loginResult["size"] << endl;
                        loginResultStr = loginResult.toStyledString();
                        dp.writeMsg(fd, 0, loginResultStr, SEND_FW);
                        break;
                    }
                    case GET_CNT: {
                        cout << "GET_CNT..." << endl;
                        string baseMsg = online.getUserName(account) + "(" + to_string(account) + ")获取剩余数量:";

                        string downmsg = dp.readTextContent(fd, dataLength);
                        Json::Reader jsonReader;
                        Json::Value msg;
                        jsonReader.parse(downmsg, msg);
                        string order = msg["order"].asString();
                        string cnt = msg["cnt"].asString();
                        cout << "order:" << order << endl;
                        cout << "cnt:" << cnt << endl;

                        Json::Value loginResult;
                        string cntstr;
                        if (us.isTaskOrderRegister(order)) {
                            vector<string> orderdata = us.CheckTaskOrder(order);
                            int currCnt = std::stoi(orderdata[3]);
                            cout << "currCnt:" << currCnt << endl;
                            cntstr = std::to_string(currCnt);
                            // loginResult["remainCnt"] = currCnt;
                            // 更新cnt
                        } else {
                            cout << "order not exist." << endl;
                            // loginResult["remainCnt"] = -1;
                            cntstr = "-1";
                        }
                        
                        loginResult["remainCnt"] = aes.aes_256_cbc_encode(cntstr);
                        string loginResultStr = loginResult.toStyledString();
                        dp.writeMsg(fd, 0, loginResultStr, SEND_CNT);
                        break;
                    }
                    case RECEIVE_DOWN_RET: {
                        cout << "RECEIVE_DOWN_RET..." << endl;
                        string baseMsg = online.getUserName(account) + "(" + to_string(account) + ")发送烧录结果:";

                        cout << "account:" << account << endl;
                        cout << "username:" << online.getUserName(account) << endl;
                        cout << "baseMsg:" << baseMsg << endl;

                        string downmsg = dp.readTextContent(fd, dataLength);
                        Json::Reader jsonReader;
                        Json::Value msg;
                        jsonReader.parse(downmsg, msg);
                        string order = msg["order"].asString();
                        string cnt = msg["cnt"].asString();
                        string ret = msg["ret"].asString();
                        cout << "order:" << order << endl;
                        cout << "cnt:" << cnt << endl;
                        cout << "ret:" << ret << endl;

                        Json::Value loginResult;
                        if (ret == "succeed" && us.isTaskOrderRegister(order)) {
                            vector<string> orderdata = us.CheckTaskOrder(order);
                            int currCnt = std::stoi(orderdata[3]);
                            cout << "currCnt:" << currCnt << endl;
                            if (currCnt > 0) {
                                --currCnt;
                                us.updateOrderCnt(order, currCnt);
                                cout << "update cnt:" << currCnt << endl;
                            } else {
                                cout << "order cnt = 0." << endl;
                                loginResult["downAck"] = RECEIVE_DOWN_ORDER_CNT_EQ0;
                            }
                            // 更新cnt
                        } else {
                            cout << "order not exist." << endl;
                            loginResult["downAck"] = RECEIVE_DOWN_ORDER_NOT_EXIST;
                        }

                        loginResult["downAck"] = RECEIVE_DOWN_RET_SUCCESS;
                        string loginResultStr = loginResult.toStyledString();
                        dp.writeMsg(fd, 0, loginResultStr, RECEIVE_DOWN_RET);
                        break;
                    }
                }

            }
        }
    }

    close(lfd);
    return 0;
}
