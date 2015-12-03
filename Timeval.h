#ifndef CTIMEVAL_H
#define CTIMEVAL_H

#include "LockAuto.h"

// (32bit)2147483647 / 31536000 = 68       1970 + 68 = 2038year
class CTimeVal
{
public:
    CTimeVal();
    CTimeVal(struct timeval tm);
    CTimeVal(signed long tv_sec, signed long tv_usec);
    ~CTimeVal();

private:

public:
    signed long m_tv_sec;
    signed long m_tv_usec;
    CTimeVal operator + (CTimeVal &tm);
    CTimeVal operator - (CTimeVal &tm);

    bool getHereTime();
    signed long getHereSec();
    signed long getHereUSec();
    signed long getHereTotalMSec();
    signed long getHereTotalUSec();

public:
    time_t m_now;
    struct tm *m_pTmnow;

    bool getNowTime();
    // Year	- 1900.
    int getNowYear();
    // Month.	[0-11]
    int getNowMonth();
    // Day.		[1-31]
    int getNowDay();
    // Hours.	[0-23]
    int getNowHour();
    // Minutes.	[0-59]
    int getNowMinute();
    // Seconds.	[0-60] (1 leap second)
    int getNowSecond();
    // Day of week.	[0-6]
    int getNowWeekOfDay();
    // Days in year.[0-365]
    int getNowYearOfDay();
};

#endif // CTIMEVAL_H
