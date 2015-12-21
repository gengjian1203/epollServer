#ifndef __CSOCKET_PACKET_H__
#define __CSOCKET_PACKET_H__

#include "LockAuto.h"

// |Length  |CMDID  |nCheck |
// |int     |int    |int    |
struct stCSocketPacketHead
{
    unsigned int nLength;
    unsigned int nCMD;
    unsigned int nCheck;
};

class CSocketPacket
{
public:
    CSocketPacket();
    ~CSocketPacket();

private:
    void* m_pClient;

private:
    char* m_pData;
    char* m_pTail;
    bool m_bCompleted;
    bool m_bFinished;
    CLock m_lock;

    int getFreeSize();
    bool checkFreeSize(unsigned int nSize);

public:
    bool setClient(void* pClient);
    void* getClient();

public:
    bool isCompleted();
    bool isChecked();
    char* getTotalData();
    char*& setTotalData();
    bool formatData();
    bool formatTail();

    bool set_CMD(int nCMD);
    bool push_bool(bool bBool);
    bool push_int(int nInt);
    bool push_long(long nLong);
    bool push_float(float fFloat);
    bool push_double(double fDouble);
    bool push_byteArray(char *pByte, int nByteSize);
    bool push_finish();

    int get_CMD();
    int get_Length();
    int get_Check();
    bool pop_bool();
    int pop_int();
    long pop_long();
    float pop_float();
    double pop_double();
    bool pop_byteArray(char*& pByte, int& nByteSize);

};

#endif // CSOCKETPACKET_H
