#ifndef __CMD_PROCESS_H__
#define __CMD_PROCESS_H__

#include "SocketPacket.h"

class CCMDProcess
{
private:
    CCMDProcess();
    ~CCMDProcess();

public:
    static CCMDProcess* getInstance();
    void Proc(CSocketPacket* pPacket);

};

#endif // CMDPROCESS_H
