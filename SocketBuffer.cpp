#include "SocketBuffer.h"

CSocketBuffer::CSocketBuffer(void)
    : m_nBufferSize(0x00)
    , m_pHead(0x00)
    , m_pRead(0x00)
    , m_pWrite(0x00)
{
    CLockAuto autolock(m_lock);

    m_pHead = (char*)malloc(sizeof(char) * SIZE_PACKET_MAX * 2);

    if (0x00 != m_pHead)
    {
        m_pRead = m_pHead;
        m_pWrite = m_pHead;
        m_nBufferSize = sizeof(char) * SIZE_PACKET_MAX * 2;
    }

}

CSocketBuffer::~CSocketBuffer(void)
{

}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketBuffer::GetFreeSize
/// 功   能：获取环形缓冲区剩余空间大小
/// 参   数：void
/// 返回 值：int                        public
////////////////////////////////////////////////////////////////////////////////
int CSocketBuffer::GetFreeSize()
{
    int nTmp = m_pRead - m_pWrite;
    if (nTmp <= 0)
    {
        nTmp += m_nBufferSize;
    }

    return nTmp;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketBuffer::GetBusySize
/// 功   能：获取环形缓冲区已用空间大小
/// 参   数：void
/// 返回 值：int                        public
////////////////////////////////////////////////////////////////////////////////
int CSocketBuffer::GetBusySize()
{
    int nTmp = m_pWrite - m_pRead;
    if (nTmp < 0)
    {
        nTmp += m_nBufferSize;
    }

    return nTmp;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketBuffer::SetLimitByteSize
/// 功   能：设置环形缓冲区总共空间大小
/// 参   数：int nLimit                设置大小（入口）
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketBuffer::SetLimitByteSize(int nLimit)
{
    CLockAuto autolock(m_lock);

    if (0x00 != m_pHead)
    {
        free(m_pHead);
        m_pHead = 0x00;
    }
    m_pHead = (char*)malloc(sizeof(char) * nLimit);

    if (0x00 != m_pHead)
    {
        m_pRead = m_pHead;
        m_pWrite = m_pHead;
        m_nBufferSize = sizeof(char) * nLimit;
        return true;
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketBuffer::GetNextPackSize()
/// 功   能：获取下一个包的长度大小
/// 参   数：void
/// 返回 值：int                        public
////////////////////////////////////////////////////////////////////////////////
int CSocketBuffer::GetNextPackSize()
{
    CLockAuto autolock(m_lock);

    int nResult = 0;
    int nInt = sizeof(int);
    if (GetBusySize() < sizeof(int))
    {
        return 0;
    }
    cout << m_pRead - m_pHead << "%%%%" << (int)*m_pRead << "^^^" <<endl;

    int nRight = m_pHead + m_nBufferSize - m_pRead;
    if (nRight < nInt)
    {
        int nLeftSize = nInt - nRight;
        if (nRight != 0)
        {
            memcpy((char*)&nResult, m_pRead, nRight);
        }
        memcpy((char*)&nResult + nRight, m_pHead, nLeftSize);
    }
    else
    {
        memcpy((char*)&nResult, m_pRead, nInt);

    }

    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketBuffer::PushByte
/// 功   能：将nBufSize长度的pBuf码流压入环形缓冲区
/// 参   数：char* pBuf                准备压入的码流信息（入口）（深拷贝）
///         int nBufSize              准备压入的码流大小（入口）
/// 返回 值：bool                      public
////////////////////////////////////////////////////////////////////////////////
bool CSocketBuffer::PushByte(char* pBuf, int nBufSize)
{
    CLockAuto autolock(m_lock);

    // 写入数据长度非法则返回
    if (nBufSize <= 0)
    {
        return false;
    }
    // 缓冲区数据不足读取长度则返回
    int nFreeSize = GetFreeSize();
    if (nFreeSize <= nBufSize)
    {
        return false;
    }
    // 写入数据
    int nRightSize = m_pHead + m_nBufferSize - m_pWrite;
    if (nRightSize < nBufSize)
    {
        int nLeftSize = nBufSize - nRightSize;
        if (nRightSize != 0)
        {
            memcpy(m_pWrite, pBuf, nRightSize);
        }
        memcpy(m_pHead, pBuf + nRightSize, nLeftSize);
        m_pWrite = m_pHead + nLeftSize;
    }
    else
    {
        memcpy(m_pWrite, pBuf, nBufSize);
        m_pWrite += nBufSize;

    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketBuffer::PopByte
/// 功   能：从环形缓冲区中取出nBufSize大小的码流pBuf
/// 参   数：char* pBuf                准备弹出环形缓冲区的指针（出口）（深拷贝）
///         int nBufSize              准备弹出环形缓冲区的数据大小（入口）
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketBuffer::PopByte(char* pBuf, int nBufSize)
{
    CLockAuto autolock(m_lock);

    // 读取数据长度非法则返回
    if (nBufSize <= 0)
    {
        return false;
    }
    // 缓冲区数据不足读取长度则返回
    int nBusySize = GetBusySize();
    if (nBusySize < nBufSize)
    {
        return false;
    }
    // 读取数据
    int nRightSize = m_pHead + m_nBufferSize - m_pRead;
    if (nRightSize < nBufSize)
    {
        int nLeftSize = nBufSize - nRightSize;
        if (nRightSize != 0)
        {
            memcpy(pBuf, m_pRead, nRightSize);
        }
        memcpy(pBuf + nRightSize, m_pHead, nLeftSize);
        m_pRead = m_pHead + nLeftSize;
    }
    else
    {
        memcpy(pBuf, m_pRead, nBufSize);
        m_pRead += nBufSize;

    }

    return true;
}

bool CSocketBuffer::isHasPacket()
{

    CLockAuto autolock(m_lock);

    // 制作下个包的长度保存在nResult中
    int nResult = 0;
    int nInt = sizeof(int);
    if (GetBusySize() < sizeof(int))
    {
        return false;
    }
    //cout << m_pRead - m_pHead << "%%%%" << (int)*m_pRead << "^^^" <<endl;

    int nRight = m_pHead + m_nBufferSize - m_pRead;
    if (nRight < nInt)
    {
        int nLeftSize = nInt - nRight;
        if (nRight != 0)
        {
            memcpy((char*)&nResult, m_pRead, nRight);
        }
        memcpy((char*)&nResult + nRight, m_pHead, nLeftSize);
    }
    else
    {
        memcpy((char*)&nResult, m_pRead, nInt);

    }

    // 读取数据长度非法则返回
    if (nResult <= 0)
    {
        return false;
    }
    // 缓冲区数据不足读取长度则返回
    int nBusySize = GetBusySize();
    if (nBusySize < nResult)
    {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSocketBuffer::PopPacket
/// 功   能：根据封包结构算出该封包大小，弹出下个封包的码流
/// 参   数：char* pBuf                弹出包的码流指针（出口）（深拷贝）
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSocketBuffer::PopPacket(char* pBuf)
{
    CLockAuto autolock(m_lock);

    // 制作下个包的长度保存在nResult中
    int nResult = 0;
    int nInt = sizeof(int);
    if (GetBusySize() < sizeof(int))
    {
        return false;
    }
    //cout << m_pRead - m_pHead << "%%%%" << (int)*m_pRead << "^^^" <<endl;

    int nRight = m_pHead + m_nBufferSize - m_pRead;
    if (nRight < nInt)
    {
        int nLeftSize = nInt - nRight;
        if (nRight != 0)
        {
            memcpy((char*)&nResult, m_pRead, nRight);
        }
        memcpy((char*)&nResult + nRight, m_pHead, nLeftSize);
    }
    else
    {
        memcpy((char*)&nResult, m_pRead, nInt);

    }

    // 读取数据长度非法则返回
    if (nResult <= 0)
    {
        return false;
    }
    // 缓冲区数据不足读取长度则返回
    int nBusySize = GetBusySize();
    if (nBusySize < nResult)
    {
        return false;
    }
    // 读取数据
    int nRightSize = m_pHead + m_nBufferSize - m_pRead;
    if (nRightSize < nResult)
    {
        int nLeftSize = nResult - nRightSize;
        if (nRightSize != 0)
        {
            memcpy(pBuf, m_pRead, nRightSize);
        }
        memcpy(pBuf + nRightSize, m_pHead, nLeftSize);
        m_pRead = m_pHead + nLeftSize;
    }
    else
    {
        memcpy(pBuf, m_pRead, nResult);
        m_pRead += nResult;

    }

    return true;
}
