#ifndef __CLOCK_H__
#define __CLOCK_H__

#include "global.h"

class CLock
{
public:
    CLock(void);
    ~CLock(void);

private:
    pthread_mutex_t m_Lock;

public:
    bool Lock();
    bool UnLock();
};


#endif
