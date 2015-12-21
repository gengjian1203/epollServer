#include "SocketPacket.h"

CSocketPacket::CSocketPacket()
    : m_bCompleted(true)
    , m_bFinished(false)
{
    int nTypeSize = sizeof(stCSocketPacketHead);

    m_pData = (char*)malloc(SIZE_PACKET_MAX);
    memset(m_pData, 0x00, SIZE_PACKET_MAX);

    m_pTail = m_pData + nTypeSize;
    ((stCSocketPacketHead*)m_pData)->nLength = nTypeSize;

}

CSocketPacket::~CSocketPacket()
{
    if (0x00 != m_pData)
    {
        free(m_pData);
        m_pData = 0x00;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::getFreeSize
/// 功   能：获取包内空闲空间大小
/// 参   数：void
/// 返回 值：int                        private
////////////////////////////////////////////////////////////////////////////////
int CSocketPacket::getFreeSize()
{

    return SIZE_PACKET_MAX - get_Length();
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::checkFreeSize
/// 功   能：确认空余大小是否足够插入nSize大小的数据
/// 参   数：unsigned int nSize        预备插入数据大小（入口）
/// 返回 值：bool                       private
////////////////////////////////////////////////////////////////////////////////
bool CSocketPacket::checkFreeSize(unsigned int nSize)
{
    return ((getFreeSize() - sizeof(int)) >= nSize);
}


bool CSocketPacket::setClient(void* pClient)
{
    m_pClient = pClient;
    return true;
}

void* CSocketPacket::getClient()
{
    return m_pClient;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::isCompleted
/// 功   能：发包前确认包是否封装完整
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketPacket::isCompleted()
{
    return (m_bCompleted && m_bFinished);
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::isChecked
/// 功   能：收包后确认包是否验证通过
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketPacket::isChecked()
{
    bool bb2 = (get_Check() == (*(m_pData + get_Length() - 4) ^ GLODEN_KEY));
    return (bb2);
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::getTotalData
/// 功   能：获取包内数据（用以传入发送函数）
/// 参   数：void
/// 返回 值：char*                      public
////////////////////////////////////////////////////////////////////////////////
char* CSocketPacket::getTotalData(void)
{
    return m_pData;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::setTotalData
/// 功   能：设置包内数据（用以传入接收函数）
/// 参   数：void
/// 返回 值：char*&                     public
////////////////////////////////////////////////////////////////////////////////
char*& CSocketPacket::setTotalData(void)
{
    formatData();
    return m_pData;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::formatData
/// 功   能：格式化包内数据
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketPacket::formatData(void)
{
    CLockAuto autolock(m_lock);
    int nTypeSize = sizeof(stCSocketPacketHead);

    memset(m_pData, 0x00, SIZE_PACKET_MAX);

    m_pTail = m_pData + nTypeSize;
    ((stCSocketPacketHead*)m_pData)->nLength = nTypeSize;

    m_bCompleted = true;
    m_bFinished = false;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::formatData
/// 功   能：格式化包内指向指针（尽量避免使用，易造成指针混乱）
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketPacket::formatTail(void)
{
    CLockAuto autolock(m_lock);
    m_pTail = m_pData + sizeof(stCSocketPacketHead);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::set_CMD
/// 功   能：设置该封包的命令号
/// 参   数：int nCMD                  命令号（入口）
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketPacket::set_CMD(int nCMD)
{
    CLockAuto autolock(m_lock);
    ((stCSocketPacketHead*)m_pData)->nCMD = nCMD;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::push_bool
/// 功   能：向该封包压入一个bool型变量
/// 参   数：bool bBool
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketPacket::push_bool(bool bBool)
{
    CLockAuto autolock(m_lock);
    if (!checkFreeSize(sizeof(bool)))
    {
        m_bCompleted = false;
        return false;
    }

    //
    int nTypeSize = sizeof(bool);
    memcpy(m_pTail, &bBool, nTypeSize);
    //
    m_pTail += nTypeSize;
    ((stCSocketPacketHead*)m_pData)->nLength += nTypeSize;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::push_int
/// 功   能：向该封包压入一个int型变量
/// 参   数：int nInt
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketPacket::push_int(int nInt)
{
    CLockAuto autolock(m_lock);
    if (!checkFreeSize(sizeof(int)))
    {
        m_bCompleted = false;
        return false;
    }
    //
    int nTypeSize = sizeof(int);
    memcpy(m_pTail, &nInt, nTypeSize);
    //
    m_pTail += nTypeSize;
    ((stCSocketPacketHead*)m_pData)->nLength += nTypeSize;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::push_long
/// 功   能：向该封包压入一个long型变量
/// 参   数：long nLong
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketPacket::push_long(long nLong)
{
    CLockAuto autolock(m_lock);
    if (!checkFreeSize(sizeof(long)))
    {
        m_bCompleted = false;
        return false;
    }
    //
    int nTypeSize = sizeof(long);
    memcpy(m_pTail, &nLong, nTypeSize);
    //
    m_pTail += nTypeSize;
    ((stCSocketPacketHead*)m_pData)->nLength += nTypeSize;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::push_float
/// 功   能：向该封包压入一个float型变量
/// 参   数：float fFloat
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketPacket::push_float(float fFloat)
{
    CLockAuto autolock(m_lock);
    if (!checkFreeSize(sizeof(float)))
    {
        m_bCompleted = false;
        return false;
    }
    //
    int nTypeSize = sizeof(float);
    memcpy(m_pTail, &fFloat, nTypeSize);
    //
    m_pTail += nTypeSize;
    ((stCSocketPacketHead*)m_pData)->nLength += nTypeSize;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::push_double
/// 功   能：向该封包压入一个double型变量
/// 参   数：double fDouble
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketPacket::push_double(double fDouble)
{
    CLockAuto autolock(m_lock);
    if (!checkFreeSize(sizeof(double)))
    {
        m_bCompleted = false;
        return false;
    }
    //
    int nTypeSize = sizeof(double);
    memcpy(m_pTail, &fDouble, nTypeSize);
    //
    m_pTail += nTypeSize;
    ((stCSocketPacketHead*)m_pData)->nLength += nTypeSize;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::push_byteArray
/// 功   能：向该封包压入一个字符流
/// 参   数：char *pByte
///          int nByteSize
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketPacket::push_byteArray(char *pByte, int nByteSize)
{
    CLockAuto autolock(m_lock);
    if (!checkFreeSize(nByteSize + sizeof(int)))
    {
        m_bCompleted = false;
        return false;
    }
    // Add StrLen
    int nTypeSize = sizeof(int);
    memcpy(m_pTail, &nByteSize, nTypeSize);
    m_pTail += nTypeSize;
    ((stCSocketPacketHead*)m_pData)->nLength += nTypeSize;

    // Add Str
    memcpy(m_pTail, pByte, nByteSize);
    m_pTail += nByteSize;
    ((stCSocketPacketHead*)m_pData)->nLength += nByteSize;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::push_finish
/// 功   能：对封包制作包尾
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketPacket::push_finish(void)
{
    CLockAuto autolock(m_lock);
    // 确认剩余空间
    if(getFreeSize() <= sizeof(int))
    {
        m_bCompleted = false;
        return false;
    }
    // 制作包尾
    int nTypeSize = sizeof(int);
    int nL = ((stCSocketPacketHead*)m_pData)->nLength + nTypeSize;
    memcpy(m_pTail, &nL, nTypeSize);
    //
    m_pTail += nTypeSize;
    ((stCSocketPacketHead*)m_pData)->nLength += nTypeSize;

    // 制作审核位
    ((stCSocketPacketHead*)m_pData)->nCheck = (get_Length() ^ GLODEN_KEY);

    m_bFinished = true;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::get_CMD
/// 功   能：取出该封包的帧号
/// 参   数：void
/// 返回 值：int                        public
////////////////////////////////////////////////////////////////////////////////
int CSocketPacket::get_CMD(void)
{
    return ((stCSocketPacketHead*)m_pData)->nCMD;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::get_Length
/// 功   能：获取该封包的长度
/// 参   数：void
/// 返回 值：int                        public
////////////////////////////////////////////////////////////////////////////////
int CSocketPacket::get_Length(void)
{
    return ((stCSocketPacketHead*)m_pData)->nLength;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::get_Check
/// 功   能：获取该封包的审核位
/// 参   数：void
/// 返回 值：int                        public
////////////////////////////////////////////////////////////////////////////////
int CSocketPacket::get_Check(void)
{
    return ((stCSocketPacketHead*)m_pData)->nCheck;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::pop_bool
/// 功   能：从该封包中顺序得到一个bool型变量
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketPacket::pop_bool(void)
{
    CLockAuto autolock(m_lock);
    bool* pTmp = (bool*)m_pTail;
    m_pTail += sizeof(bool);

    return *pTmp;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::pop_int
/// 功   能：从该封包中顺序得到一个int型变量
/// 参   数：void
/// 返回 值：int                        public
////////////////////////////////////////////////////////////////////////////////
int CSocketPacket::pop_int(void)
{
    CLockAuto autolock(m_lock);
    int* pTmp = (int*)m_pTail;
    m_pTail += sizeof(int);

    return *pTmp;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::pop_long
/// 功   能：从该封包中顺序得到一个long型变量
/// 参   数：void
/// 返回 值：long                       public
////////////////////////////////////////////////////////////////////////////////
long CSocketPacket::pop_long(void)
{
    CLockAuto autolock(m_lock);
    long* pTmp = (long*)m_pTail;
    m_pTail += sizeof(long);

    return *pTmp;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::pop_float
/// 功   能：从该封包中顺序得到一个float型变量
/// 参   数：void
/// 返回 值：float                      public
////////////////////////////////////////////////////////////////////////////////
float CSocketPacket::pop_float(void)
{
    CLockAuto autolock(m_lock);
    float* pTmp = (float*)m_pTail;
    m_pTail += sizeof(float);

    return *pTmp;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::pop_double
/// 功   能：从该封包中顺序得到一个double型变量
/// 参   数：void
/// 返回 值：double                     public
////////////////////////////////////////////////////////////////////////////////
double CSocketPacket::pop_double(void)
{
    CLockAuto autolock(m_lock);
    double* pTmp = (double*)m_pTail;
    m_pTail += sizeof(double);

    return *pTmp;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketPacket::pop_byteArray
/// 功   能：从该封包中顺序得到一个字符流(对指针赋值，指向封包申请的内存空间)
/// 参   数：char*& pByte
///          int& nByteSize
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketPacket::pop_byteArray(char*& pByte, int& nByteSize)
{
    CLockAuto autolock(m_lock);
    // get Len
    nByteSize = *(int*)m_pTail;
    m_pTail += sizeof(int);
    // get data
    pByte = m_pTail;
    m_pTail += nByteSize;

    return true;
}
