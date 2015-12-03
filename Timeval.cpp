#include "Timeval.h"

CTimeVal::CTimeVal()
    : m_now(0)
    , m_pTmnow(0x00)
{
    m_tv_sec = 0;
    m_tv_usec = 0;
}

CTimeVal::CTimeVal(timeval tm)
    : m_now(0)
    , m_pTmnow(0x00)
{
    m_tv_sec = tm.tv_sec;
    m_tv_usec = tm.tv_usec;
}

CTimeVal::CTimeVal(signed long tv_sec, signed long tv_usec)
    : m_now(0)
    , m_pTmnow(0x00)
{
    m_tv_sec = tv_sec;
    m_tv_usec = tv_usec;
}

CTimeVal::~CTimeVal()
{

}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::operator +
/// 功   能：对CTimeVal类型数据重载加号的处理
/// 参   数：CTimeVal &tm
/// 返回 值：CTimeVal                   public
////////////////////////////////////////////////////////////////////////////////
CTimeVal CTimeVal::operator + (CTimeVal &tm)
{
    CTimeVal tmp;
    tmp.m_tv_sec = m_tv_sec + tm.m_tv_sec;
    tmp.m_tv_usec = m_tv_usec + tm.m_tv_usec;
    if (tmp.m_tv_usec >= 1000000)
    {
        tmp.m_tv_sec += 1;
        tmp.m_tv_usec -= 1000000;
    }
    return tmp;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::operator -
/// 功   能：对CTimeVal类型数据重载减号的处理
/// 参   数：CTimeVal &tm
/// 返回 值：CTimeVal                   public
////////////////////////////////////////////////////////////////////////////////
CTimeVal CTimeVal::operator - (CTimeVal &tm)
{
    CTimeVal tmp;
    tmp.m_tv_sec = m_tv_sec - tm.m_tv_sec;
    tmp.m_tv_usec = m_tv_usec - tm.m_tv_usec;
    if (tmp.m_tv_usec < 0)
    {
        tmp.m_tv_sec -= 1;
        tmp.m_tv_usec += 1000000;
    }
    return tmp;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::getHereTime
/// 功   能：获取当前系统时间，为1970年到现在的秒数、及零头微秒
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CTimeVal::getHereTime(void)
{
    struct timeval tm;
    gettimeofday(&tm, NULL);
    m_tv_sec = tm.tv_sec;
    m_tv_usec = tm.tv_usec;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::getHereSec
/// 功   能：存储时间秒数
/// 参   数：void
/// 返回 值：signed long                 public
////////////////////////////////////////////////////////////////////////////////
signed long CTimeVal::getHereSec(void)
{
    return m_tv_sec;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::getHereUSec
/// 功   能：存储时间秒数的零头微秒
/// 参   数：void
/// 返回 值：signed long                 public
////////////////////////////////////////////////////////////////////////////////
signed long CTimeVal::getHereUSec(void)
{
    return m_tv_usec;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::getHereTotalMSec
/// 功   能：存储时间的总共毫秒
/// 参   数：void
/// 返回 值：signed long                 public
////////////////////////////////////////////////////////////////////////////////
signed long CTimeVal::getHereTotalMSec()
{
    getHereTime();
    return (m_tv_sec * 1000) + (long)(m_tv_usec / 1000);
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::getHereNowUSec
/// 功   能：存储时间的总共微秒
/// 参   数：void
/// 返回 值：signed long                 public
////////////////////////////////////////////////////////////////////////////////
signed long CTimeVal::getHereTotalUSec()
{
    return (m_tv_sec * 1000000) + m_tv_usec;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::getNowTime
/// 功   能：获取当前系统时间，并将时间保存在成员变量中等待读取
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CTimeVal::getNowTime()
{
    time(&m_now);
    m_pTmnow = localtime(&m_now);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::getNowYear
/// 功   能：得到类中保存时间的年
/// 参   数：void
/// 返回 值：int                        public
////////////////////////////////////////////////////////////////////////////////
int CTimeVal::getNowYear()
{
    if (m_pTmnow)
    {
        return m_pTmnow->tm_year + 1900;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::getNowMonth
/// 功   能：得到类中保存时间的月
/// 参   数：void
/// 返回 值：int                        public
////////////////////////////////////////////////////////////////////////////////
int CTimeVal::getNowMonth()
{
    if (m_pTmnow)
    {
        return m_pTmnow->tm_mon + 1;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::getNowDay
/// 功   能：得到类中保存时间的日
/// 参   数：void
/// 返回 值：int                        public
////////////////////////////////////////////////////////////////////////////////
int CTimeVal::getNowDay()
{
    if (m_pTmnow)
    {
        return m_pTmnow->tm_mday;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::getNowHour
/// 功   能：得到类中保存时间的小时
/// 参   数：void
/// 返回 值：int                        public
////////////////////////////////////////////////////////////////////////////////
int CTimeVal::getNowHour()
{
    if (m_pTmnow)
    {
        return m_pTmnow->tm_hour;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::getNowMinute
/// 功   能：得到类中保存时间的分钟
/// 参   数：void
/// 返回 值：int                        public
////////////////////////////////////////////////////////////////////////////////
int CTimeVal::getNowMinute()
{
    if (m_pTmnow)
    {
        return m_pTmnow->tm_min;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::getNowSecond
/// 功   能：得到类中保存时间的秒
/// 参   数：void
/// 返回 值：int                        public
////////////////////////////////////////////////////////////////////////////////
int CTimeVal::getNowSecond()
{
    if (m_pTmnow)
    {
        return m_pTmnow->tm_sec;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::getNowWeekOfDay
/// 功   能：得到类中保存时间的星期
/// 参   数：void
/// 返回 值：int                        public
////////////////////////////////////////////////////////////////////////////////
int CTimeVal::getNowWeekOfDay()
{
    if (m_pTmnow)
    {
        return m_pTmnow->tm_wday;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CTimeVal::getNowYearOfDay
/// 功   能：得到类中保存时间的一年内的天数
/// 参   数：void
/// 返回 值：int                        public
////////////////////////////////////////////////////////////////////////////////
int CTimeVal::getNowYearOfDay()
{
    if (m_pTmnow)
    {
        return m_pTmnow->tm_yday;
    }
    return 0;
}
