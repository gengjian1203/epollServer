#include "LockAuto.h"

CLockAuto::CLockAuto(CLock& lock)
{
	m_pLock = &lock;
	m_pLock->Lock();
}

CLockAuto::~CLockAuto(void)
{
	m_pLock->UnLock();
}
