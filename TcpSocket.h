#ifndef __TCPSOCKET_H__
#define __TCPSOCKET_H__

#include "SocketPacket.h"
#include "SocketClient.h"
#include "SqlManager.h"
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
    static bool EpollELWork(map<int, CSocketClient*>* pMapBuffer, epoll_event* events, int number, int epollfd, int listenfd);
    static void* ThreadEpollWork(void* param);
    // Config
    char m_strCfgIP[20];
    int m_nCfgPort;
    int m_nCfgLimitNum;

//    // 单位秒，监测空闲列表时间间隔，在空闲队列中超过TASK_DESTROY_INTERVAL时间的任务将被自动销毁
//    const int CHECK_IDLE_TASK_INTERVAL = 300;
//    // 单位秒，任务自动销毁时间间隔
//    const int TASK_DESTROY_INTERVAL = 60;
//    // 单位微秒，监控线程池是否为空时间间隔,微秒
//    const int IDLE_CHECK_POLL_EMPTY = 500;
//    // 单位秒，线程池线程空闲自动退出时间间隔 ,5分钟
//    const int  THREAD_WAIT_TIME_OUT = 300;

    // Socket连接管理
    int m_fdListen;
    struct sockaddr_in m_addr;
    int m_fdEpoll;
    epoll_event m_events[RLIMIT_MAX];
    // 传递数据结构
    bool m_bThreadEpollWork;
    map<int, void*> m_mapBuffer;
    stuThreadParam m_parWork;

    struct rlimit m_rt;                             // 用意设置连接数量

    bool LoadConfig();
    bool SetLimitMax();
    bool ShowSocketStatus();
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
