#include "loginform.h"
#include "ui_loginform.h"
#include "widget.h"
#include "writeclient.h"
#include "readclient.h"
#include "registerform.h"
#include "download.h"

#include <QMessageBox>
#include <QTimer>
#include "aescrypto.h"
#include "rsacrypto.h"
#include "sha256.h"

LoginForm::LoginForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginForm)
{
    ui->setupUi(this);
    connect(ui->loginPushButton, &QPushButton::clicked, this, &LoginForm::connectServer);
    connect(ui->registerPushButton, &QPushButton::clicked, this, [=](){
        writeClient = WriteClient::getTcpSocketClient();
        if(writeClient == nullptr){
            qDebug() << "连接服务器超时";
            QMessageBox::information(this, "登录提示", "连接服务器超时");
            return;
        }
        RegisterForm * rf = new RegisterForm;
        rf->loginForm = this;
        rf->show();
        this->hide();
    });

}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::connectServer(){
    qDebug() << "-------------开始连接服务器----------------";
    writeClient = WriteClient::getTcpSocketClient();
    if(writeClient == nullptr){
        qDebug() << "连接服务器超时";
        QMessageBox::information(this, "登录提示", "连接服务器超时");
        return;
    }
    QString account = ui->userIdLineEdit->text();
    qDebug() << "账号:" << account;
    QString password = ui->passwordLineEdit->text();
    qDebug() << "密码:" << password;

    RsaCrypto rsa;
    Sha256 sha;

    QJsonObject information;
    rsa.public_encrypt((unsigned char *)account.toLatin1().data(), account.size());
    information.insert("account", QString::fromStdString(rsa.encryptedstr));
    rsa.public_encrypt((unsigned char *)password.toLatin1().data(), password.size());
    information.insert("password", QString::fromStdString(rsa.encryptedstr));

    aespassword = AesCrypto::GenAesKey();
    rsa.public_encrypt((unsigned char *)aespassword.c_str(), aespassword.size());
    information.insert("aeskey", QString::fromStdString(rsa.encryptedstr));
    QJsonDocument doc;
    doc.setObject(information);
    string loginMsg =doc.toJson(QJsonDocument::Compact).toStdString();
    qDebug() << "loginMsg:" << QString::fromStdString(loginMsg);
    writeClient->writeText(account.toUtf8().toInt(), loginMsg, LOGIN);
    writeClient->getTcpClient()->waitForReadyRead(-1);
    qDebug() << "收到后台确认信息---------------------";
    QJsonObject serverInfoObj = writeClient->readServerMsg();
    qDebug() << serverInfoObj["status"];
    int status = serverInfoObj["status"].toInt();
    qDebug() << "status:" << status;
    if(status == LOGIN_SUCCESS){
        readClient = ReadClient::getTcpSocketClient();
        qDebug() << "登陆成功, 开始read端---------";
        //readClient->writeText(account, "", READ);
        Download* downloadWindow = new Download;
        downloadWindow->loginForm = this;
        user = new User(account.toUtf8().toInt(), serverInfoObj["username"].toString());
        downloadWindow->user = user;
        qDebug() << "account:" << account << "->" << "username:" << serverInfoObj["username"].toString();
        downloadWindow->disPlayUserInfo();
        downloadWindow->show();
        readClient->writeText(account.toUtf8().toInt(), "", READ); // 这里给服务器发一次请求，获取当前
        this->hide();
        qDebug() << "*************";
    }
    else if(status == LOGIN_FAIL){
        QMessageBox::information(this, "登录提示", "账号或密码错误,请重试");
    }
    else if(status == LOGIN_EXIST){
        QMessageBox::information(this, "登录提示", "该账号已登录，不可重复登录");
    }
}

void LoginForm::closeEvent ( QCloseEvent * e )
{
    qDebug() << "loginform关闭了";
    int userId = user != nullptr? user->getUserId():0;

    if(readClient != nullptr){
        qDebug() << "readClient关闭了";
        writeClient->writeText(userId, "", LOGOUT);
        //确保登出信息能够及时发到服务端
        sleep(300);
        writeClient->getTcpClient()->disconnectFromHost();
        sleep(100);
        readClient->getTcpClient()->disconnectFromHost();
        sleep(100);
    }
    else if(writeClient != nullptr){
        qDebug() << "writeClient关闭了";
        //确保关闭信息能够及时发到服务端
        writeClient->writeText(userId, "", CLOSE);
        sleep(300);
        writeClient->getTcpClient()->disconnectFromHost();
        sleep(100);
    }
}

void LoginForm::sleep(int msec){
    QEventLoop eventloop;
    QTimer::singleShot(msec, &eventloop, SLOT(quit()));
    eventloop.exec();
}


