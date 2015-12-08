#include "CMDProcess.h"

CCMDProcess::CCMDProcess()
{

}

CCMDProcess::~CCMDProcess()
{

}

CCMDProcess* CCMDProcess::getInstance()
{
    static CCMDProcess instance;
    return &instance;
}

void CCMDProcess::Proc(CSocketPacket* pPacket)
{
    int nCMD = pPacket->get_CMD();
    int nResult = pPacket->pop_long();
    for (int i = 0; i < 1000; i++)
    {
        int j = 9898;
    }
    LOG("child thread tid = %d.nResult = %d.\n", pthread_self(), nResult);

//    // RingBuffer, ThreadPoor,

//    CSocketPacket packSend;
//    int nCMD = 0, nPa1 = 0, nPa2 = 0, nStrLen = 0;
//    char* pStr = 0x00;
//    long lTime = 0;

//    nCMD = pack.get_CMD();
//    nPa1 = pack.pop_int();
//    nPa2 = pack.pop_int();
//    pack.pop_byteArray(pStr, nStrLen);
//    lTime = pack.pop_long();
//    LOG("Recv CMD:%d, nPa1:%d, nPa2:%d, Str:%s, nTime:%d,",
//        nCMD, nPa1, nPa2, pStr, lTime);

//    packSend.formatData();
//    packSend.set_CMD(nCMD);
//    packSend.push_int(nPa1);
//    packSend.push_int(nPa2);
//    packSend.push_int(nPa1 + nPa2);
//    packSend.push_byteArray(pStr, nStrLen);
//    packSend.push_long(lTime);
//    packSend.push_finish();
//    if (packSend.isCompleted())
//    {
//        int nRet = send(sockfd, packSend.getTotalData(), packSend.get_Length(), 0);
//        if (nRet < 0)
//        {
//            LOG("Send Error.\n");
//        }
//    }
//    else
//    {
//        LOG("pack is not completed.\n");
//    }

    delete pPacket;
    return;
}
