#include "ch341.h"
#include <iostream>
#include <QDebug>

using namespace std;

Ch341::Ch341(ULONG index, bool open, UCHAR idev) : mIndex(index), m_open(open), iDevice(idev)
{
    sendBuf = new UCHAR[g_maxSizeCh341];
    readBuf = new UCHAR[g_maxSizeCh341];
    SetEEPROMType(ID_24C02);
}

Ch341::~Ch341()
{
    delete[] sendBuf;
    delete[] readBuf;
    CloseDevice();
    qDebug() << "Bye Ch341~" << endl;
}

bool Ch341::ReadByte(UCHAR iAddr, UCHAR* oByte)
{
    if (!GetDeviceOpenStatus()) {
        return false;
    }
    UCHAR sendBuf[2] = { GetiDevice(), iAddr };

    if (CH341StreamI2C(mIndex, 2, (UCHAR*)sendBuf, 1, (UCHAR*)oByte)) {
        return true;
    } else {
        return false;
    }
}

bool Ch341::WriteByte(UCHAR iAddr, UCHAR iByte)
{
    if (!GetDeviceOpenStatus()) {
        return false;
    }
    UCHAR sendBuf[3] = { GetiDevice(), iAddr, iByte };
    if (CH341StreamI2C(mIndex, 3, (UCHAR*)&sendBuf[0], 0, NULL)) {
        return true;
    } else {
        return false;
    }
}

bool Ch341::ReadByte(USHORT iAddr, UCHAR* oByte)
{
    if (!GetDeviceOpenStatus()) {
        return false;
    }

    UCHAR sendBuf[3] = { GetiDevice(), (UCHAR)((iAddr >> 8) & 0xFF), (UCHAR)(iAddr & 0xFF) };

    if (CH341StreamI2C(mIndex, 3, (UCHAR*)sendBuf, 1, (UCHAR*)oByte)) {
        return true;
    } else {
        return false;
    }
}

bool Ch341::WriteByte(USHORT iAddr, UCHAR iByte)
{
    if (!GetDeviceOpenStatus()) {
        return false;
    }
    UCHAR sendBuf[4] = { GetiDevice(), (UCHAR)((iAddr >> 8) & 0xFF), (UCHAR)(iAddr & 0xFF), iByte };
    if (CH341StreamI2C(mIndex, 4, (UCHAR*)&sendBuf[0], 0, NULL)) {
        return true;
    } else {
        return false;
    }
}

bool Ch341::ReadBytes(UCHAR iAddr, ULONG iLength, PUCHAR iBuffer)
{
    if (!GetDeviceOpenStatus()) {
        return false;
    }

    UCHAR sendBuf[2] = { GetiDevice(), iAddr };
    if (CH341StreamI2C(mIndex, 2, (UCHAR*)&sendBuf[0], iLength, (UCHAR*)iBuffer)) {
        return true;
    } else {
        return false;
    }
}

bool Ch341::WriteBytes(UCHAR iAddr, ULONG iLength, PUCHAR oBuffer)
{
    if (!GetDeviceOpenStatus()) {
        return false;
    }

    auto sendBuf = make_unique<UCHAR[]>(iLength + 2);
    sendBuf[0] = GetiDevice();
    sendBuf[1] = iAddr;
    memcpy(&sendBuf[2], oBuffer, iLength);
    if (CH341StreamI2C(mIndex, iLength + 2, (UCHAR*)&sendBuf[0], 0, NULL)) {
        return true;
    } else {
        return false;
    }
}

bool Ch341::ReadBytes(USHORT iAddr, ULONG iLength, PUCHAR iBuffer)
{
    if (!GetDeviceOpenStatus()) {
        return false;
    }

    UCHAR sendBuf[3] = { GetiDevice(), (UCHAR)((iAddr >> 8) & 0xFF), (UCHAR)(iAddr & 0xFF) };
    if (CH341StreamI2C(mIndex, 3, (UCHAR*)&sendBuf[0], iLength, (UCHAR*)iBuffer)) {
        return true;
    } else {
        return false;
    }
}

bool Ch341::WriteBytes(USHORT iAddr, ULONG iLength, PUCHAR oBuffer)
{
    if (!GetDeviceOpenStatus()) {
        return false;
    }

    auto sendBuf = make_unique<UCHAR[]>(iLength + 3);
    sendBuf[0] = GetiDevice();
    sendBuf[1] = (iAddr >> 8) & 0xFF;
    sendBuf[2] = iAddr & 0xFF;
    memcpy(&sendBuf[3], oBuffer, iLength);
    if (CH341StreamI2C(mIndex, iLength + 3, (UCHAR*)&sendBuf[0], 0, NULL)) {
        return true;
    } else {
        return false;
    }
}

bool Ch341::ReadEEPROM(ULONG iAddr, ULONG iLength, PUCHAR oBuffer)
{
    if (!GetDeviceOpenStatus()) {
        return false;
    }
    if (CH341ReadEEPROM(mIndex, GetEEPROMType(), iAddr, iLength, oBuffer)) {
        return true;
    } else {
        return false;
    }
}

bool Ch341::WriteEEPROM(ULONG iAddr, ULONG iLength, PUCHAR iBuffer)
{
    if (!GetDeviceOpenStatus()) {
        return false;
    }
    if (CH341WriteEEPROM(mIndex, GetEEPROMType(), iAddr, iLength, iBuffer)) {
        return true;
    } else {
        return false;
    }
}

bool Ch341::OpenDevice()
{
//    if (LoadLibrary("CH341DLL.DLL") == NULL) {
//        SetDeviceOpenStatus(false);
//        return false;
//    }
    if (GetDeviceOpenStatus()) { // 已打开
        return true;
    }
    if (CH341OpenDevice(mIndex) == INVALID_HANDLE_VALUE) {
        SetDeviceOpenStatus(false);
        qDebug() << "CH341打开失败!\n";
    } else {
        SetDeviceOpenStatus(true);
        qDebug() << "ch341 version:" << GetDrvVersion() << endl;
    }
    return GetDeviceOpenStatus();
}

bool Ch341::CloseDevice()
{
    CH341CloseDevice(mIndex);
    SetDeviceOpenStatus(false);
    qDebug() << "CloseDevice";
    return true;
}

/*
0x00: 20k
0x01: 100k 默认
0x02: 400k
0x03: 750k
*/
bool Ch341::SetSpeed(ULONG speed)
{
    if (!GetDeviceOpenStatus()) {
        return false;
    }
    if (!CH341SetStream(mIndex, speed)) {
        return false;
    }
    CloseDevice();
    return OpenDevice();
}
