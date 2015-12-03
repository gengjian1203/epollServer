#ifndef __CSOCKET_CLIENT_H__
#define __CSOCKET_CLIENT_H__

#include "SocketBuffer.h"

class CSocketClient
{
public:
    CSocketClient();
    ~CSocketClient();

private:
    int m_nActiveSign;
    char m_strLoginDate[SIZE_CLIENT_DATE_MAX];
    struct sockaddr_in m_address;
    CSocketBuffer m_buffer;


public:
    bool Init();
    bool Destory();

    bool setAddress(struct sockaddr_in* src);
    struct sockaddr_in* getAddress();
    bool setLoginDate(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond);
    char* getLoginDate();

    CSocketBuffer* GetBuffer();

};

#endif // SOCKETCLIENT_H
