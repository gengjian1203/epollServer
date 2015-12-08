#ifndef __TCPSOCKET_H__
#define __TCPSOCKET_H__

#include "SocketClient.h"
#include "SqlManager.h"
#include "ThreadPoolManager.h"
#include "CMDProcess.h"
#include "Timeval.h"
#include "Config.h"

// 待实现内存池、线程池
class TCPSocket
{
public:
    TCPSocket();
    ~TCPSocket();

private:
    static bool SetNoBlock(int fd);
    static bool SignInfd(CSocketClient* pClient);
    static bool Addfd(struct sockaddr_in* pAddr, map<int, CSocketClient*>* pMapBuffer, int epollfd, int fd);
    static bool SignOutfd(CSocketClient* pClient);
    static bool Delfd(map<int, CSocketClient*>* pMapBuffer, int epollfd, int fd);
    static void MyProcFunc(void* pArg);
    static bool EpollELWork(stuThreadParam* pStu, int number);
    static void* ThreadEpollWork(void* param);
    // Config
    char m_strCfgIP[SIZE_CLIENT_IP_MAX];
    int m_nCfgPort;
    int m_nCfgLimitNum;
    int m_nSizeTask;
    int m_nSizeThreadPoolMax;
    int m_nSizeThreadPoolPre;
    // Socket连接管理
    int m_fdListen;
    struct sockaddr_in m_addr;
    int m_fdEpoll;
    epoll_event m_events[RLIMIT_MAX];
    // 传递数据结构
    bool m_bThreadEpollWork;
    map<int, void*> m_mapBuffer;
    stuThreadParam m_parWork;
    CThreadPoolManager m_mgrThreadPool;             // 线程池管理

    struct rlimit m_rt;                             // 用以设置连接数量

    bool LoadConfig();
    bool SetLimitMax();
    bool ShowSocketStatus();
    bool ThreadPollInit();
    bool MaskSignal();
    bool CreateSocket();
    bool CreateEpoll();
    bool BindSocket();
    bool ListenSocket();

public:
    bool Init();
    bool Run();
    bool Destory();
};

#endif // TCPSOCKET_H
