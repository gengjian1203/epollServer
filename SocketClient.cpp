#include "SocketClient.h"

CSocketClient::CSocketClient()
{
    m_nActiveSign = 0;
    memset(m_strLoginDate, 0, sizeof(m_strLoginDate));
    memset(&m_address, 0, sizeof(sockaddr_in));

}

CSocketClient::~CSocketClient()
{

}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketClient::Init
/// 功   能：初始化对象
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketClient::Init()
{

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketClient::Destory
/// 功   能：销毁对象
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketClient::Destory()
{

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketClient::setAddress
/// 功   能：保存来访客户端的IP及Port
/// 参   数：struct sockaddr_in* src   通过accept函数获得的客户端地址信息（入口）（深拷贝）
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketClient::setAddress(struct sockaddr_in* src)
{
    memcpy(&m_address, src, sizeof(struct sockaddr_in));

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketClient::getAddress
/// 功   能：获取客户端的IP及Port
/// 参   数：void
/// 返回 值：struct sockaddr_in*        public
////////////////////////////////////////////////////////////////////////////////
struct sockaddr_in* CSocketClient::getAddress()
{

    return &m_address;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketClient::setLoginDate
/// 功   能：以字符串形式保存客户端登录时间
/// 参   数：int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond
///         年、月、日、时、分、秒（入口）
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketClient::setLoginDate(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond)
{
    //2011-11-11 11:11:11
    sprintf(m_strLoginDate, "%d-%d-%d %d:%d:%d", nYear, nMonth, nDay, nHour, nMinute, nSecond);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketClient::getLoginDate
/// 功   能：获取客户端登录时间字符串
/// 参   数：void
/// 返回 值：char*                       public
////////////////////////////////////////////////////////////////////////////////
char* CSocketClient::getLoginDate()
{

    return m_strLoginDate;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketClient::GetBuffer
/// 功   能：获取绑定该客户端的环形缓冲区指针
/// 参   数：void
/// 返回 值：CSocketBuffer*              public
////////////////////////////////////////////////////////////////////////////////
CSocketBuffer* CSocketClient::GetBuffer()
{

    return &m_buffer;
}
