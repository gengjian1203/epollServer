#ifndef __CMD_PROCESS_H__
#define __CMD_PROCESS_H__

#include "SocketClient.h"
#include "SocketPacket.h"
#include "AgreeCMD.h"

typedef int (* PF_COMMAND_HANDLER)(int nCmd, CSocketClient* pClient, CSocketPacket* pPacket);

class CCMDProcess
{
private:
    CCMDProcess();
    ~CCMDProcess();

private:
    map<int, PF_COMMAND_HANDLER> m_mapCmdHandlerTable;

private:
    static int HandlerTest(int nCMD, CSocketClient* pClient, CSocketPacket* pPacket);
    static int HandlerHeartBeat(int nCMD, CSocketClient* pClient, CSocketPacket* pPacket);

public:
    static CCMDProcess* getInstance();
    void Proc(CSocketPacket* pPacket);

};

#endif // CMDPROCESS_H
