#ifndef __CSOCKET_BUFFER_H__
#define __CSOCKET_BUFFER_H__

#include "LockAuto.h"

class CSocketBuffer
{
public:
    CSocketBuffer(void);
    ~CSocketBuffer(void);

private:
    int m_nBufferSize;
    char* m_pHead;
    char* m_pRead;
    char* m_pWrite;
    CLock m_lock;

public:
    int GetFreeSize();
    int GetBusySize();

    bool SetLimitByteSize(int nLimit);
    int GetNextPackSize();
    bool PushByte(char* pBuf, int nBufSize);
    bool PopByte(char* pBuf, int nBufSize);

    bool isHasPacket();
    bool PopPacket(char* pBuf);

};

#endif // SOCKETBUFFER_H
