#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QMainWindow>
#include "writeclient.h"
#include "loginform.h"
#include "readclient.h"
#include "readthread.h"
#include "user.h"
#include "ch341.h"
#include "downthread.h"
#include <QThread>
#include <QTimer>
#include "rsacrypto.h"
#include "aescrypto.h"

namespace Ui {
class Download;
}

#define MAX_THREAD 4

class Download : public QMainWindow
{
    Q_OBJECT

public:
    RsaCrypto rsa;
    AesCrypto aes;
    explicit Download(QWidget *parent = nullptr);
    ~Download();
    void disPlayUserInfo() {};
    LoginForm * loginForm;
    User * user = nullptr;

    std::shared_ptr<Ch341> ch341_m;

    void SetCursorToEnd();
    void MainThreadDealProgress(int per, QString fw, QString type, int channel, bool ret);

    void DealClose(); //窗口关闭槽函数
    void CreateMythread();
    void RegisterUsb();
//    bool nativeEvent(const QByteArray & eventType, void * message, long*result);
    bool SendDownRet(QString order, int cnt, QString ret);
    int SendGetCnt(QString order, int cnt);
    vector<string> static GetMacAddress();

private slots:
    void on_btnOpenDongle_clicked();

    void on_commregvalueHexInput_editingFinished();

    void on_commregvalueDecInput_editingFinished();

    void on_btnReadCommRegister_clicked();

    void on_btnWriteCommRegister_clicked();

    void on_btnLoadRxHex_clicked();

    void on_btnDownloadRxHex_clicked();

    void on_ReadFw_clicked();

    void on_btnDownloadHex_clicked();

    void on_btnExit_clicked();

    void on_btnSend_clicked();

    void on_btnSendImage_clicked();

    void on_btnGetHex_clicked();

    void on_taskOrderInput_editingFinished();

    void on_lineEdit_6_editingFinished();

    void on_pushButton_26_clicked();

private:
    Ui::Download *ui;
    WriteClient * writeClient = nullptr;
    ReadClient * readClient = nullptr;
    void closeEvent(QCloseEvent * e);
    ReadThread *thread;

    DownThread *myT[MAX_THREAD]; //自定义线程对象
    QThread *downthread[MAX_THREAD]; //子线程
    QTimer *myTimer; //声明变量
    string fwmsg;
    int remainCnt;
    bool isOrderLegal;

signals:
    void CommStartDownload1(std::shared_ptr<Ch341> ch341, int channel);
    void CommStartDownload2(std::shared_ptr<Ch341> ch341, int channel);
    void CommStartDownload3(std::shared_ptr<Ch341> ch341, int channel);
    void CommStartDownload4(std::shared_ptr<Ch341> ch341, int channel);
    void CommStartReadFw1(std::shared_ptr<Ch341> ch341, int channel, int len);
    void CommStartReadFw2(std::shared_ptr<Ch341> ch341, int channel, int len);
    void CommStartReadFw3(std::shared_ptr<Ch341> ch341, int channel, int len);
    void CommStartReadFw4(std::shared_ptr<Ch341> ch341, int channel, int len);
};

#endif // DOWNLOAD_H
