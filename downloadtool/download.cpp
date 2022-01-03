#include "download.h"
#include "ui_download.h"
#include <QStyleFactory>
#include <QFileDialog>
#include <QFile>
#include <QLabel>
#include <windows.h>
#include <dbt.h>
#include <QNetworkInterface>

Download::Download(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Download)
{
    ui->setupUi(this);
    qApp->setStyle(QStyleFactory::create("Fusion"));
    fwmsg.clear();
    remainCnt = 0;
    isOrderLegal = false;
    aes.password = aespassword;

    //获取客户端连接
    writeClient = WriteClient::getTcpSocketClient();
    //获取读端
    qDebug() << "开始read端1---------";
    readClient = ReadClient::getTcpSocketClient();

    thread = new ReadThread;
    thread->start();

    connect(ui->btnSendDonwRet, &QPushButton::clicked, this, [=](){
        if (SendDownRet(ui->taskOrderInput->text(), ui->cntInput->text().toInt(), "succeed")) {
            int cnt = SendGetCnt(ui->taskOrderInput->text(), ui->cntInput->text().toInt());
            ui->cntInput->setText(QString::number(cnt));
        } else {
            ui->commDownRet->setText("FAIL");
            ui->commDownRet->setStyleSheet("color:red");
            qDebug() << "发送烧录结果失败！";
        }
    });

    connect(ui->btnGetHexCnt, &QPushButton::clicked, this, [=](){
       int cnt = SendGetCnt(ui->taskOrderInput->text(), ui->cntInput->text().toInt());
       ui->cntInput->setText(QString::number(cnt));
    });

    //发送文本数据
    connect(ui->btnSend, &QPushButton::clicked, this, [=](){
       QString text = ui->inputTextEdit->toPlainText();
       if(text.trimmed().length() == 0){
           QMessageBox::information(this,"发送提示","不能发送空内容");
           return;
       }
       writeClient->writeText(user->getUserId(), text.toStdString());
       ui->inputTextEdit->clear();
    });

    //发送图片
    connect(ui->btnSendImage, &QPushButton::clicked, this, [=](){
        QString fileName=QFileDialog::getOpenFileName(this,tr("Open Image"),".",tr("Image Files(*.png *.jpg *jpeg *.bmp)"));
        qDebug() << "文件名---------------" << fileName;
        QTextCodec *code = QTextCodec::codecForName("GB2312");//解决中文路径问题
        string imagePath = code->fromUnicode(fileName).data();
        //string imagePath = fileName.toStdString();
        if(imagePath != ""){
            writeClient->writeImage(user->getUserId(), imagePath);
            qDebug() << "发送文件成功";
        }
    });

    connect(readClient, &ReadClient::textDisplayAble, this, [=](){
       ui->contentListWidget->addItem(QString::fromStdString(readClient->getTextContent()));
    });

    ui->contentListWidget->setIconSize(QSize(200,200));
    connect(readClient, &ReadClient::imageDisplayAble, this, [=](){
       QListWidgetItem * pic = new QListWidgetItem;
       pic->setIcon(QIcon(QString::fromStdString(readClient->getImagePath())));
       ui->contentListWidget->addItem(pic);
    });

    connect(readClient, &ReadClient::noticeDisplayAble, this, [=](){
       ui->noticeListWidget->addItem(QString::fromStdString(readClient->getNoticeContent()));
    });

    connect(readClient, &ReadClient::onlineDisplayAble, this, [=](){
        ui->onlineListWidget->clear();
        QVariantList onlineList = QJsonDocument::fromJson(QString::fromStdString(readClient->getTextContent()).toUtf8()).toVariant().toList();
        for(auto & user: onlineList){
            QVariantMap map = user.toMap();
            QString element = map["account"].toString() + "\t" + map["username"].toString() + "\t" + map["loginTime"].toString();
            ui->onlineListWidget->addItem(element);
        }
        ui->onlineLabel->setText("当前在线人数(" + QString::number(onlineList.size()) + "人)");
    });

    // 暂时没用
    connect(readClient, &ReadClient::fwReceiveAble, this, [=](){
        SendGetCnt(ui->taskOrderInput->text(), ui->cntInput->text().toInt());
        fwmsg.clear();
        if (readClient->getTextContent() == "order not exist") {
            isOrderLegal = false;
            QMessageBox::information(this, "获取任务令", "任务令无效，请更换任务令！");

        } else {
            fwmsg = readClient->getTextContent();
            isOrderLegal = true;
        }
    });

    // 断网
    connect(&readClient->tcpClient, &QTcpSocket::disconnected, this, [=](void) -> void {
        qDebug() << "readClient 断开";
        thread->exit();
        this->loginForm->close();
        this->close();
    });

    connect(&writeClient->tcpClient, &QTcpSocket::disconnected, this, [=](void) -> void {
        qDebug() << "writeClient 断开";
        thread->exit();
        this->loginForm->close();
        this->close();
    });

    ch341_m = std::make_shared<Ch341>(0, false, 0x60);
    CreateMythread();
    RegisterUsb();
}

Download::~Download()
{
    ch341_m->CloseDevice();
    delete ui;
    delete user;
}

void Download::closeEvent ( QCloseEvent * e )
{
    thread->exit();
    this->loginForm->close();
    //this->close();
}

void Download::CreateMythread()
{
    //自定义类对象，分配空间，不可以指定父对象
    for (int i = 0; i < MAX_THREAD; i++) {
        myT[i] = new DownThread;
        //创建子线程
        downthread[i] = new QThread(this);
        //把自定义模块添加到子线程
        myT[i]->moveToThread(downthread[i]);

        //启动子线程，但是，并没有启动线程处理函数
        downthread[i]->start();

    //    myTimer = new QTimer(this);
    //    myTimer->start(100);

    //    connect(myTimer, &QTimer::timeout, myT, &MyThread::CommDownload);

        //只要定时器启动，自动触发timeout() //线程处理函数，必须通过signal - slot 调用
    //    connect(ui->btnDownloadRxHex, &QPushButton::clicked, myT, &MyThread::MythreadDeal);



        connect(myT[i], &DownThread::SendCommDownloadProgress, this, &Download::MainThreadDealProgress);

        connect(myT[i], &DownThread::SendCommReadProgress, this, &Download::MainThreadDealProgress);

        connect(this, &Download::destroyed, this, &Download::DealClose);
    }
    connect(this, &Download::CommStartDownload1, myT[0], &DownThread::ThreadCommDownload);
    connect(this, &Download::CommStartDownload2, myT[1], &DownThread::ThreadCommDownload);
    connect(this, &Download::CommStartDownload3, myT[2], &DownThread::ThreadCommDownload);
    connect(this, &Download::CommStartDownload4, myT[3], &DownThread::ThreadCommDownload);
    connect(this, &Download::CommStartReadFw1, myT[0], &DownThread::TreadCommReadFw);
    connect(this, &Download::CommStartReadFw2, myT[0], &DownThread::TreadCommReadFw);
    connect(this, &Download::CommStartReadFw3, myT[0], &DownThread::TreadCommReadFw);
    connect(this, &Download::CommStartReadFw4, myT[0], &DownThread::TreadCommReadFw);
}

bool Download::SendDownRet(QString order, int cnt, QString ret)
{
    bool result = false;
    QJsonObject information;
    information.insert("order", order);
    information.insert("ret", ret);
    information.insert("cnt", cnt);
    QJsonDocument doc;
    doc.setObject(information);
    string downRetMsg =doc.toJson(QJsonDocument::Compact).toStdString();
    qDebug() << "order:" << QString::fromStdString(downRetMsg);

    for (int i = 0; i < 3 && result == false; i++) {
        writeClient->writeText(user->getUserId(), downRetMsg, SEND_DOWN_RET);
        result = writeClient->getTcpClient()->waitForReadyRead(-1);
    }

    if (!result) {
        return false;
    }
    qDebug() << "收到后台确认信息---------------------";
    QJsonObject serverInfoObj = writeClient->readServerMsg();
    qDebug() << serverInfoObj["downAck"];
    int downAck = serverInfoObj["downAck"].toInt();
    qDebug() << "downAck:" << downAck;
    if (downAck == 0) {
        return true;
    }
    return false;
}

int Download::SendGetCnt(QString order, int cnt)
{
    bool ret = false;
    QJsonObject information;
    information.insert("order", order);
    information.insert("cnt", cnt);
    QJsonDocument doc;
    doc.setObject(information);
    string getCntMsg =doc.toJson(QJsonDocument::Compact).toStdString();
    qDebug() << "getCntMsg:" << QString::fromStdString(getCntMsg);

    for (int i = 0; i < 3 && ret == false; i++) {
        writeClient->writeText(user->getUserId(), getCntMsg, SEND_GET_CNT);
        ret = writeClient->getTcpClient()->waitForReadyRead(-1);
    }

    qDebug() << "ret:" << ret << endl;
    if (!ret) {
        this->remainCnt = -1;
        ui->cntInput->setText(QString::number(remainCnt));
        ui->lineEdit_7->setText(QString::number(remainCnt));
        return remainCnt;
    }
    qDebug() << "收到后台确认信息---------------------";

    QJsonObject serverInfoObj = writeClient->readServerMsg();
    qDebug() << serverInfoObj["remainCnt"];

    string cnttemp = serverInfoObj.value("remainCnt").toString().toStdString();
    string cntstr = aes.aes_256_cbc_decode(cnttemp);
//    int remainCnt = serverInfoObj["remainCnt"].toInt();
    int remainCnt = std::stoi(cntstr);
    qDebug() << "remainCnt:" << remainCnt;
    this->remainCnt = remainCnt;
    ui->cntInput->setText(QString::number(remainCnt));
    ui->lineEdit_7->setText(QString::number(remainCnt));
    return remainCnt;
}

void Download::RegisterUsb()
{
    static const GUID GUID_DEVINTERFACE_LIST[] =
    {
    // GUID_DEVINTERFACE_USB_DEVICE
    { 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },
//    // GUID_DEVINTERFACE_DISK
//    { 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },
//    // GUID_DEVINTERFACE_HID,
//    { 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },
//    // GUID_NDIS_LAN_CLASS
//    { 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } },
//    // GUID_DEVINTERFACE_COMPORT
//    { 0x86e0d1e0, 0x8089, 0x11d0, { 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73 } },
//    // GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR
//    { 0x4D36E978, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } },
//    // GUID_DEVINTERFACE_PARALLEL
//    { 0x97F76EF0, 0xF883, 0x11D0, { 0xAF, 0x1F, 0x00, 0x00, 0xF8, 0x00, 0x84, 0x5C } },
//    // GUID_DEVINTERFACE_PARCLASS
//    { 0x811FC6A5, 0xF728, 0x11D0, { 0xA5, 0x37, 0x00, 0x00, 0xF8, 0x75, 0x3E, 0xD1 } },
    };

    //注册插拔事件
    HDEVNOTIFY hDevNotify;
    DEV_BROADCAST_DEVICEINTERFACE NotifacationFiler;
    ZeroMemory(&NotifacationFiler, sizeof(DEV_BROADCAST_DEVICEINTERFACE));
    NotifacationFiler.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotifacationFiler.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    for(int i = 0;i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++)
    {
        NotifacationFiler.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];//GetCurrentUSBGUID();
        hDevNotify = RegisterDeviceNotification(HANDLE(this->winId()), &NotifacationFiler, DEVICE_NOTIFY_WINDOW_HANDLE);
        if(!hDevNotify)
        {
            GetLastError();
            qDebug() << "注册失败" <<endl;
        }
    }
}

// 有bug，拔出插入设备闪退
//bool Download::nativeEvent(const QByteArray &eventType, void *message, long *result)
//{
//    MSG* msg = reinterpret_cast<MSG*>(message);//第一层解算
//    UINT msgType = msg->message;
//    if(msgType==WM_DEVICECHANGE)
//    {
//        PDEV_BROADCAST_HDR lpdb = PDEV_BROADCAST_HDR(msg->lParam);//第二层解算
//        switch (msg->wParam) {
//        case DBT_DEVICEARRIVAL:
//            if(lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
//            {
//                PDEV_BROADCAST_DEVICEINTERFACE pDevInf  = PDEV_BROADCAST_DEVICEINTERFACE(lpdb);
//                QString device_name = "插入设备(name)："+QString::fromWCharArray(pDevInf->dbcc_name,int(pDevInf->dbcc_size)).toUtf8();
//                qDebug() << device_name;
//                if (device_name.toStdString().find("VID_1A86&PID_5512") != string::npos) {
//                    qDebug() << "IIC Dongle Arrival";
////                    ui->labelRxDongle->setText("IIC Dongle Arrival");
//                    ui->commdongleCheckLable->setText("IIC Dongle Arrival");
//                }
//            }
//            break;
//        case DBT_DEVICEREMOVECOMPLETE:
//            if(lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
//            {
//                PDEV_BROADCAST_DEVICEINTERFACE pDevInf  = PDEV_BROADCAST_DEVICEINTERFACE(lpdb);
//                QString device_name = "移除设备(name)："+QString::fromWCharArray(pDevInf->dbcc_name,int(pDevInf->dbcc_size)).toUtf8();
//                qDebug() << device_name;
//                if (device_name.toStdString().find("VID_1A86&PID_5512") != string::npos) {
//                    qDebug() << "IIC Dongle Removed";
////                    ui->labelRxDongle->setText("IIC Dongle Removed");
//                    ui->commdongleCheckLable->setText("IIC Dongle Removed");
//                }
//            }
//            break;
//        default:
//            if(lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
//            {
//                PDEV_BROADCAST_DEVICEINTERFACE pDevInf  = PDEV_BROADCAST_DEVICEINTERFACE(lpdb);
//                QString device_name = "插入设备(name)："+QString::fromWCharArray(pDevInf->dbcc_name,int(pDevInf->dbcc_size)).toUtf8();
//                qDebug() << device_name;
//                if (device_name.toStdString().find("VID_1A86&PID_5512") != string::npos) {
//                    qDebug() << "IIC Dongle Arrival";
////                    ui->labelRxDongle->setText("IIC Dongle Arrival");
//                    ui->commdongleCheckLable->setText("IIC Dongle Arrival");
//                }
//            }
//            break;
//        }
//    }
//    return false;
//}

void Download::DealClose()
{
    for (int i = 0; i < MAX_THREAD; i++) {
        //退出子线程
        downthread[i]->quit();
        //回收资源
        downthread[i]->wait();
        delete myT[i];
    }
}

void Download::SetCursorToEnd()
{
    //移动光标到末尾
    QTextCursor cursor = ui->commtextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->commtextEdit->setTextCursor(cursor);
}

vector<string> Download::GetMacAddress()
{
    vector<string> address;
    foreach(QNetworkInterface aInterface, QNetworkInterface::allInterfaces()) {
        address.push_back(aInterface.hardwareAddress().toStdString());
    }
    return address;
}

void Download::MainThreadDealProgress(int per, QString fw, QString type, int channel, bool ret)
{
    if (type == "write" && ret == false) {
        SendDownRet(ui->lineEdit_6->text(), ui->cntInput->text().toInt(), "fail");
    }
    if (fw != "") {
        ui->commtextEdit->append(fw);
        ui->textEdit_2->append(fw);
    }
    if (channel == 1) {
        ui->progressBar->setValue(per);
    } else if (channel == 2) {
        ui->progressBar_2->setValue(per);
    } else if (channel == 3) {
        ui->progressBar_3->setValue(per);
    } else if (channel == 4) {
        ui->progressBar_4->setValue(per);
    }

    ui->commdownprogressBar->setValue(per);

    if (per == 100) {
        ui->btnDownloadRxHex->setEnabled(true);
        ui->pushButton_26->setEnabled(true);
        ui->ReadFw->setEnabled(true);
        if (type == "write") {
            if (SendDownRet(ui->lineEdit_6->text(), ui->cntInput->text().toInt(), "succeed")) {
                if (this->remainCnt > 0) {
                    --this->remainCnt;
                    ui->cntInput->setText(QString::number(this->remainCnt));
                    ui->lineEdit_7->setText(QString::number(this->remainCnt));
                }
                if (channel == 1) {
                    ui->label_4->setText("PASS");
                    ui->label_4->setStyleSheet("color:green");
                } else if (channel == 2) {
                    ui->label_5->setText("PASS");
                    ui->label_5->setStyleSheet("color:green");
                } else if (channel == 3) {
                    ui->label_6->setText("PASS");
                    ui->label_6->setStyleSheet("color:green");
                } else if (channel == 4) {
                    ui->label_51->setText("PASS");
                    ui->label_51->setStyleSheet("color:green");
                }
                ui->commDownRet->setText("PASS");
                ui->commDownRet->setStyleSheet("color:green");
                qDebug() << "发送烧录结果成功！";
            } else {
                if (channel == 1) {
                    ui->label_4->setText("FAIL");
                    ui->label_4->setStyleSheet("color:red");
                } else if (channel == 2) {
                    ui->label_5->setText("FAIL");
                    ui->label_5->setStyleSheet("color:red");
                } else if (channel == 3) {
                    ui->label_6->setText("FAIL");
                    ui->label_6->setStyleSheet("color:red");
                } else if (channel == 4) {
                    ui->label_51->setText("FAIL");
                    ui->label_51->setStyleSheet("color:red");
                }
                ui->commDownRet->setText("FAIL");
                ui->commDownRet->setStyleSheet("color:red");
                qDebug() << "发送烧录结果失败！";
            }
        }
    }
//    update(); //更新窗口
}

void Download::on_btnOpenDongle_clicked()
{
    if(!ch341_m->OpenDevice()) {
        ui->commtextEdit->append("Device open fail");
    }
    ui->commtextEdit->append("Device open success");
    SetCursorToEnd();
}

void Download::on_commregvalueHexInput_editingFinished()
{
    uint16_t dataHex = ui->commregvalueHexInput->text().toInt(Q_NULLPTR, 16);
    ui->commregvalueDecInput->setText(QString::number(dataHex));
}

void Download::on_commregvalueDecInput_editingFinished()
{
    uint16_t dataDec = ui->commregvalueDecInput->text().toInt();
    ui->commregvalueHexInput->setText(QString::number(dataDec, 16));
}

void Download::on_btnReadCommRegister_clicked()
{
    if(!ch341_m->GetDeviceOpenStatus()) {
        ui->commtextEdit->append("Device not open");
        SetCursorToEnd();
        return;
    }

    uint8_t slaveAddr = ui->commSlavevalueDecInput->text().toUInt(Q_NULLPTR, 16);
    uint16_t regAddr = ui->commregAddrInput->text().toInt(Q_NULLPTR, 16);
    uint8_t data;

    ch341_m->SetiDevice(slaveAddr << 1);
    if (ch341_m->ReadByte(regAddr, &data)) {
        ui->commregvalueHexInput->setText(QString::number(data, 16));
        ui->commregvalueDecInput->setText(QString::number(data));
    } else {
        ui->commtextEdit->append("Read Reg Fail");
    }
    SetCursorToEnd();
}

void Download::on_btnWriteCommRegister_clicked()
{
    if(!ch341_m->GetDeviceOpenStatus()) {
        ui->commtextEdit->append("Device not open");
        SetCursorToEnd();
        return;
    }

    uint8_t slaveAddr = ui->commSlavevalueDecInput->text().toUInt(Q_NULLPTR, 16);
    uint16_t regAddr = ui->commregAddrInput->text().toInt(Q_NULLPTR, 16);
    uint8_t data = ui->commregvalueDecInput->text().toInt();

    ch341_m->SetiDevice(slaveAddr << 1);
    if (!ch341_m->WriteByte(regAddr, data)) {
        ui->commtextEdit->append("Write Reg Fail");
    }
    SetCursorToEnd();
}

void Download::on_btnLoadRxHex_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "打开固件", "../", "固件(*.hex *.bin)");
    qDebug() << path;
    ui->RxHexFwPath->setText(path);

    QFile fwFile(path);

    ch341_m->rdBuff.clear();
    if (!fwFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Open file failed.";
    } else {
        while (!fwFile.atEnd()) {
            ch341_m->rdBuff += fwFile.readLine().toStdString();
        }
    }
    QFileInfo info(fwFile);
    QString Info = "固件名：" + info.baseName();
    ui->commtextEdit->setText(Info);
    ui->commtextEdit->append("固件大小：" + QString::number(ch341_m->rdBuff.size()));
    ui->commtextEdit->append(QString::fromLocal8Bit(ch341_m->rdBuff.c_str()));
}

void Download::on_btnDownloadRxHex_clicked()
{
//    if(!ch341_m->GetDeviceOpenStatus()) {
//        ui->commtextEdit->append("Device not open");
//        SetCursorToEnd();
//        return;
//    }

    ui->commdownprogressBar->setValue(0);
//    uint8_t eeprom = ui->eepSelect->currentIndex();
//    ch341_m->SetEEPROMType((EEPROM_TYPE)eeprom);

    CommStartDownload1(ch341_m, 1);

    ui->btnDownloadRxHex->setEnabled(false);
    ui->ReadFw->setEnabled(false);
}

void Download::on_ReadFw_clicked()
{
//    if(!ch341_m->GetDeviceOpenStatus()) {
//        ui->commtextEdit->append("Device not open");
//        SetCursorToEnd();
//        return;
//    }

    int readSize = ui->commreadLen->text().toInt();
    if (readSize <= 0) {
        return;
    }
    ui->commtextEdit->setText("");
    ui->commdownprogressBar->setValue(0);
//    uint8_t eeprom = ui->eepSelect->currentIndex();
//    ch341_m->SetEEPROMType((EEPROM_TYPE)eeprom);

    CommStartReadFw1(ch341_m, 1, readSize);

    ui->btnDownloadRxHex->setEnabled(false);
    ui->ReadFw->setEnabled(false);
}

void Download::on_btnDownloadHex_clicked()
{
    qDebug() << "on_btnDownloadHex_clicked";

    int remainCnt = SendGetCnt(ui->taskOrderInput->text(), ui->cntInput->text().toInt());

    if (!isOrderLegal) {
        qDebug() << "任务令无效！";
        QMessageBox::information(this, "烧录提示", "任务令无效，请更换任务令！");
        return;
    }
    if (remainCnt <= 0) {
        qDebug() << "任务令数量不足！";
        QMessageBox::information(this, "烧录提示", "任务令数量不足，请更换任务令！");
        return;
    }

    ch341_m->rdBuff = fwmsg;
    qDebug() << "ch341_m->rdBuff:" << QString::fromStdString(ch341_m->rdBuff);

    ui->commdownprogressBar->setValue(0);
    CommStartDownload1(ch341_m, 1);

    ui->btnDownloadRxHex->setEnabled(false);
    ui->ReadFw->setEnabled(false);
}

void Download::on_btnExit_clicked()
{
    qDebug() << "on_btnExit_clicked";
    thread->exit();
    this->close();
    this->loginForm->close();
}

void Download::on_btnSend_clicked()
{

}

void Download::on_btnSendImage_clicked()
{

}

void Download::on_btnGetHex_clicked()
{
    qDebug() << "on_btnGetHex_clicked";

    on_taskOrderInput_editingFinished();
}

void Download::on_taskOrderInput_editingFinished()
{
    bool ret = false;
    qDebug() << "on_taskOrderInput_editingFinished";

    QString taskOrder = ui->taskOrderInput->text();

    qDebug() << "taskOrder:" << taskOrder;
    if(taskOrder.trimmed().length() == 0){
        QMessageBox::information(this, "任务令提示", "任务令不能为空！");
        return;
    }

    // 获取数量
    int cnt = SendGetCnt(ui->taskOrderInput->text(), ui->cntInput->text().toInt());

    // 获取固件
    for (int i = 0; i < 3 && ret == false; i++) {
        writeClient->writeText(user->getUserId(), taskOrder.toStdString(), SEND_GET_FW);
        ret = writeClient->getTcpClient()->waitForReadyRead(-1);
    }

    if (!ret || cnt == -1) {
        isOrderLegal = false;
        qDebug() << "获取任务令失败！";
        QMessageBox::information(this, "获取任务令", "获取任务令失败！");
        return;
    }

    qDebug() << "收到固件信息---------------------";
    fwmsg.clear();

    QJsonObject serverInfoObj = writeClient->readServerMsg();

//    fwmsg = serverInfoObj.value("fw").toString().toStdString();

    string fwtemp = serverInfoObj.value("fw").toString().toStdString();
    string fwmsg = aes.aes_256_cbc_decode(fwtemp);
//    qDebug() << "fwmsg:" << QString::fromStdString(fwmsg);

    int size = serverInfoObj["size"].toDouble();
    qDebug() << "size:" << size;

    if (fwmsg == "order not exist" || fwmsg == "open hex fail") {
        isOrderLegal = false;
        QMessageBox::information(this, "获取任务令", "任务令无效，请更换任务令！");
    } else {
        fwmsg = readClient->getTextContent();
        isOrderLegal = true;
    }
}

void Download::on_lineEdit_6_editingFinished()
{
    bool ret = false;
    qDebug() << "on_lineEdit_6_editingFinished";

    QString taskOrder = ui->lineEdit_6->text();

    qDebug() << "taskOrder:" << taskOrder;
    if(taskOrder.trimmed().length() == 0){
        QMessageBox::information(this, "任务令提示", "任务令不能为空！");
        return;
    }

    // 获取数量
    int cnt = SendGetCnt(ui->lineEdit_6->text(), ui->lineEdit_7->text().toInt());

    // 获取固件
    for (int i = 0; i < 3 && ret == false; i++) {
        writeClient->writeText(user->getUserId(), taskOrder.toStdString(), SEND_GET_FW);
        ret = writeClient->getTcpClient()->waitForReadyRead(-1);
    }

    if (!ret || cnt == -1) {
        isOrderLegal = false;
        qDebug() << "获取任务令失败！";
        QMessageBox::information(this, "获取任务令", "获取任务令失败！");
        return;
    }

    qDebug() << "收到固件信息---------------------";
    fwmsg.clear();

    QJsonObject serverInfoObj = writeClient->readServerMsg();

//    fwmsg = serverInfoObj.value("fw").toString().toStdString();

    string fwtemp = serverInfoObj.value("fw").toString().toStdString();
    string fwmsg = aes.aes_256_cbc_decode(fwtemp);
//    qDebug() << "fwmsg:" << QString::fromStdString(fwmsg);

    int size = serverInfoObj["size"].toDouble();
    qDebug() << "size:" << size;

    if (fwmsg == "order not exist" || fwmsg == "open hex fail") {
        isOrderLegal = false;
        QMessageBox::information(this, "获取任务令", "任务令无效，请更换任务令！");
    } else {
        fwmsg = readClient->getTextContent();
        isOrderLegal = true;
    }
}

void Download::on_pushButton_26_clicked()
{
    qDebug() << "on_pushButton_26_clicked";

    int remainCnt = SendGetCnt(ui->lineEdit_6->text(), ui->lineEdit_7->text().toInt());

    if (!isOrderLegal) {
        qDebug() << "任务令无效！";
        QMessageBox::information(this, "烧录提示", "任务令无效，请更换任务令！");
        return;
    }
    if (remainCnt <= 0) {
        qDebug() << "任务令数量不足！";
        QMessageBox::information(this, "烧录提示", "任务令数量不足，请更换任务令！");
        return;
    }

    ch341_m->rdBuff = fwmsg;
    qDebug() << "ch341_m->rdBuff:" << QString::fromStdString(ch341_m->rdBuff);

    ui->progressBar->setValue(0);
    ui->progressBar_2->setValue(0);
    ui->progressBar_3->setValue(0);
    ui->progressBar_4->setValue(0);

//    for (int i = 0; i < MAX_THREAD; i++) {
//        CommStartDownload(ch341_m, i + 1);
//    }
    CommStartDownload1(ch341_m, 1);
    CommStartDownload2(ch341_m, 2);
    CommStartDownload3(ch341_m, 3);
    CommStartDownload4(ch341_m, 4);

    ui->pushButton_26->setEnabled(false);
    ui->ReadFw->setEnabled(false);
}
