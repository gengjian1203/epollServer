#ifndef __CLOCK_AUTO_H__
#define __CLOCK_AUTO_H__

#include "Lock.h"

class CLockAuto
{
public:
    CLockAuto();
	CLockAuto(CLock& lock);
	~CLockAuto(void);

private:
	CLock* m_pLock;
};


#endif
