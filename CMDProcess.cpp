#include "CMDProcess.h"

CCMDProcess::CCMDProcess()
{
    // 初始化
    m_mapCmdHandlerTable[SERVER_CMD_TEST] = &HandlerTest;
    m_mapCmdHandlerTable[SERVER_CMD_HEARTBEAT] = &HandlerHeartBeat;
}

CCMDProcess::~CCMDProcess()
{

}

int CCMDProcess::HandlerTest(int nCMD, CSocketClient* pClient, CSocketPacket* pPacket)
{
    CSocketPacket packSend;
    int nPa1 = 0, nPa2 = 0, nStrLen = 0;
    char* pStr = 0x00;
    long lTime = 0;

    nPa1 = pPacket->pop_int();
    nPa2 = pPacket->pop_int();
    pPacket->pop_byteArray(pStr, nStrLen);
    lTime = pPacket->pop_long();

    packSend.formatData();
    packSend.set_CMD(nCMD);
    packSend.push_int(nPa1);
    packSend.push_int(nPa2);
    packSend.push_int(nPa1 + nPa2);
    packSend.push_byteArray(pStr, nStrLen);
    packSend.push_long(lTime);
    packSend.push_finish();
    if (packSend.isCompleted())
    {
        int nRet = send(pClient->getFD(), packSend.getTotalData(), packSend.get_Length(), 0);
        if (nRet < 0)
        {
            LOG("%d.Send Error.%s.\n", pClient->getFD(), strerror(errno));
        }
    }
    else
    {
        LOG("pack is not completed.\n");
    }

    return 0;
}

int CCMDProcess::HandlerHeartBeat(int nCMD, CSocketClient* pClient, CSocketPacket* pPacket)
{

    return 0;
}

CCMDProcess* CCMDProcess::getInstance()
{
    static CCMDProcess instance;
    return &instance;
}

void CCMDProcess::Proc(CSocketPacket* pPacket)
{
    CSocketClient* pClient = (CSocketClient*)(pPacket->getClient());
    int nCMD = pPacket->get_CMD();
    map<int, PF_COMMAND_HANDLER>::iterator itHandler = m_mapCmdHandlerTable.find(nCMD);

    if (itHandler == m_mapCmdHandlerTable.end())
    {
        LOG("%s:%d sent a unsupported command:%d.\n", inet_ntoa(pClient->getAddress()->sin_addr), htons(pClient->getAddress()->sin_port), nCMD);
    }
    else
    {
        LOG("%s:%d received command:%d. child thread tid=%d.\n", inet_ntoa(pClient->getAddress()->sin_addr), htons(pClient->getAddress()->sin_port), nCMD, pthread_self());
        PF_COMMAND_HANDLER pfHandler = 0x00;
        pfHandler = itHandler->second;
        if (pfHandler)
        {
            pfHandler(nCMD, pClient, pPacket);
        }
    }

    if (pPacket)
    {
        delete pPacket;
        pPacket = 0x00;
    }
    return;
}
