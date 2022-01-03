#ifndef CH341_H
#define CH341_H

#include <windows.h>
#include <memory>
#include <string>
#include <QMetaType>
#include "CH341DLL_GCC.h"

const int g_maxSizeCh341 = 1024;

class Ch341
{
public:
    Ch341(ULONG index, bool open, UCHAR idev);
    ~Ch341();

    ULONG mIndex;
    bool m_open;
    UCHAR iDevice;
    EEPROM_TYPE	iEepromID;
    UCHAR* sendBuf;
    UCHAR* readBuf;
    std::string rdBuff;
    std::string wtBuff;

    void SetDeviceOpenStatus(bool status) { m_open = status; }
    bool GetDeviceOpenStatus() { return m_open; }
    void SetiDevice(UCHAR addr) { iDevice = addr; }
    UCHAR GetiDevice() { return iDevice; }
    void SetEEPROMType(EEPROM_TYPE type) { iEepromID = type; }
    EEPROM_TYPE GetEEPROMType() { return iEepromID; }

    bool OpenDevice();
    bool CloseDevice();
    ULONG GetDrvVersion() { return CH341GetDrvVersion(); }
    bool SetSpeed(ULONG speed);
    bool WriteEEPROM(ULONG iAddr, ULONG iLength, PUCHAR iBuffer);
    bool ReadEEPROM(ULONG iAddr, ULONG iLength, PUCHAR oBuffer);

    bool ReadByte(UCHAR iAddr, UCHAR* oByte);
    bool WriteByte(UCHAR iAddr, UCHAR iByte);

    bool ReadByte(USHORT iAddr, UCHAR* oByte);
    bool WriteByte(USHORT iAddr, UCHAR iByte);

    bool ReadBytes(UCHAR iAddr, ULONG iLength, PUCHAR iBuffer);
    bool WriteBytes(UCHAR iAddr, ULONG iLength, PUCHAR oBuffer);

    bool ReadBytes(USHORT iAddr, ULONG iLength, PUCHAR iBuffer);
    bool WriteBytes(USHORT iAddr, ULONG iLength, PUCHAR oBuffer);
};

#endif // CH341_H
