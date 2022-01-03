#include "downthread.h"
#include <QDebug>

DownThread::DownThread(QObject *parent) : QObject(parent)
{

}

void DownThread::ThreadCommDownload(std::shared_ptr<Ch341> ch341, int channel)
{
    qDebug() << "ThreadCommDownload";

    uint32_t addr = 0x0000;
    uint32_t blockSize = 512;
    uint32_t fwLen = ch341->rdBuff.size();
    uint32_t currLen = 0;
    int currPer = 0;

    for (uint32_t i = 0; i < ch341->rdBuff.size(); i++) {
        ch341->rdBuff[i] -= (uint8_t)0x30;
    }

    for (uint32_t i = 0; i + blockSize <= fwLen; i += blockSize) {
        QThread::msleep(1);
        std::string tmp({(const char*)&ch341->rdBuff.c_str()[i], blockSize});
        QString tmpQt = QString::fromStdString(tmp);
        addr += blockSize;
        currLen += blockSize;
        currPer = currLen * 100 / fwLen;
        SendCommDownloadProgress(currPer, "", "write", channel, true);
    }

    for (uint32_t i = addr; i < fwLen; i++) {
        QThread::msleep(10);
        std::string tmp({(const char*)&ch341->rdBuff.c_str()[i], 1});
        QString tmpQt = QString::fromStdString(tmp);
        currLen++;
        currPer = currLen * 100 / fwLen;
        SendCommDownloadProgress(currPer, "", "write", channel, true);
    }
    currPer = 100;
    SendCommDownloadProgress(currPer, "", "write", channel, true);
}

void DownThread::TreadCommReadFw(std::shared_ptr<Ch341> ch341, int channel, int len)
{
    qDebug() << "CommReadFw";

    uint32_t addr = 0;
    int blockSize = 32;
    auto readBuff = std::make_unique<uint8_t[]>(blockSize);
    uint32_t currLen = 0;
    int currPer = 0;
    QString readFw;
    for (int i = 0; i + blockSize <= len; i += blockSize) {
        QThread::msleep(10);

        readFw.clear();
        for (int j = 0; j < blockSize; j++) {
            readBuff[j] += (uint8_t)0x30;
            readFw.push_back(readBuff[j]);
        }
        addr += blockSize;
        currLen += blockSize;
        currPer = currLen * 100 / len;
        SendCommReadProgress(currPer, readFw, "read", channel, true);
    }

    for (int i = addr; i < len; i++) {
        QThread::msleep(10);
        readFw.clear();
        readBuff[i - addr] += (uint8_t)0x30;
        readFw.push_back(readBuff[i - addr]);

        currLen++;
        currPer = currLen * 100 / len;
        SendCommReadProgress(currPer, readFw, "read", channel, true);
    }
}
