#include "TcpSocket.h"

TCPSocket::TCPSocket()
    : m_bThreadEpollWork(false)
    , m_nCfgPort(0)
    , m_nCfgLimitNum(0)
    , m_nSizeTask(0)
    , m_nSizeThreadPoolMax(0)
    , m_nSizeThreadPoolPre(0)
{
    memset(m_strCfgIP, 0, sizeof(m_strCfgIP));

    m_parWork.pfdListen = &m_fdListen;
    m_parWork.pfdEpoll = &m_fdEpoll;
    m_parWork.pEvents = m_events;
    m_parWork.pMapBuffer = &m_mapBuffer;
    m_parWork.pMgrThreadPool = &m_mgrThreadPool;
    m_parWork.pRLimitNum = &m_nCfgLimitNum;
    m_parWork.pOnOff = &m_bThreadEpollWork;

}

TCPSocket::~TCPSocket()
{

}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::SetNoBlock
/// 功   能：设置该Socket为非阻塞
/// 参   数：int fd
/// 返回 值：bool                       private
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::SetNoBlock(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::SignInfd
/// 功   能：更新数据库客户登录信息
/// 参   数：CSocketClient* pClient     待登录的客户Client对象指针
/// 返回 值：bool                       private
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::SignInfd(CSocketClient* pClient)
{
    char strSQL[SIZE_STRSQL_MAX];
    CTimeVal time;
    time.getNowTime();
    //LOG("In is IP:%s, Port:%d.\n", inet_ntoa(pClient->getAddress()->sin_addr), htons(pClient->getAddress()->sin_port));
    // insert into signinlog(ip, port, indate) values('192.168.1.1', 1111, '2011-11-11 11:11:11');
    pClient->setLoginDate(time.getNowYear(), time.getNowMonth(), time.getNowDay(),
                          time.getNowHour(), time.getNowMinute(), time.getNowSecond());
    sprintf(strSQL, "insert into signinlog(ip, port, indate) values('%s', %d, '%s')",
            inet_ntoa(pClient->getAddress()->sin_addr), htons(pClient->getAddress()->sin_port),
            pClient->getLoginDate());
    CSQLManager::getInstance()->Query(strSQL);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::Addfd
/// 功   能：增加客户端连接
/// 参   数：struct sockaddr_in* pAddr                 登录客户连接的地址
///         map<int, CSocketClient*>* pMapBuffer      map容器：Key为客户sockfd，Data为Client对象指针
///         int epollfd                               epoll集合，用以增加客户端
///         int fd                                    准备删除客户的socket
/// 返回 值：bool                       private
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::Addfd(struct sockaddr_in* pAddr, map<int, CSocketClient*>* pMapBuffer, int epollfd, int fd)
{
    (*pMapBuffer)[fd] = new CSocketClient;
    (*pMapBuffer)[fd]->Init();
    (*pMapBuffer)[fd]->setAddress(pAddr);

    epoll_event event;
    memset(&event, 0x00, sizeof(event));
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    SetNoBlock(fd);
    SignInfd((*pMapBuffer)[fd]);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::SignOutfd
/// 功   能：更新数据库客户登出信息
/// 参   数：CSocketClient* pClient     待登出的客户Client对象指针
/// 返回 值：bool                       private
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::SignOutfd(CSocketClient* pClient)
{
    char strSQL[SIZE_STRSQL_MAX];
    CTimeVal time;
    time.getNowTime();
    LOG("Out is IP:%s, Port:%d.\n", inet_ntoa(pClient->getAddress()->sin_addr), htons(pClient->getAddress()->sin_port));
    // update signinlog set outdate='2015-11-12 15:12:16' where ip='192.168.1.1' and port=1111 and indate='2011-11-11 00:00:00';
    sprintf(strSQL, "update signinlog set outdate='%d-%d-%d %d:%d:%d' where ip='%s' and port=%d and indate='%s'",
            time.getNowYear(), time.getNowMonth(), time.getNowDay(),
            time.getNowHour(), time.getNowMinute(), time.getNowSecond(),
            inet_ntoa(pClient->getAddress()->sin_addr), htons(pClient->getAddress()->sin_port),
            pClient->getLoginDate());
    CSQLManager::getInstance()->Query(strSQL);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::Delfd
/// 功   能：删除客户端连接
/// 参   数：map<int, CSocketClient*>* pMapBuffer      map容器：Key为客户sockfd，Data为Client对象指针
///         int epollfd                               epoll集合，用以删除客户端
///         int fd                                    准备删除客户的socket
/// 返回 值：bool                       private
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::Delfd(map<int, CSocketClient*>* pMapBuffer, int epollfd, int fd)
{
    epoll_event event;
    memset(&event, 0x00, sizeof(event));
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &event);
    SignOutfd((*pMapBuffer)[fd]);
    (*pMapBuffer)[fd]->Destory();
    delete (*pMapBuffer)[fd];

    close(fd);
    return true;
}

void TCPSocket::MyProcFunc(void* pArg)
{
    CCMDProcess::getInstance()->Proc((CSocketPacket*)pArg);
    return ;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::EpollELWork
/// 功   能：Epoll主要工作函数
/// 参   数：stuThreadParam* pStu                      传入函数的结构体指针
///         int number                                本次捕获事件数组个数
/// 返回 值：bool                       private
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::EpollELWork(stuThreadParam* pStu, int number)
{
    CSocketPacket* pPacket;
    char buf[SIZE_PACKET_MAX];

    // map容器：Key为客户sockfd，Data为Client对象指针
    map<int, CSocketClient*>* pMapBuffer = (map<int, CSocketClient*>*)pStu->pMapBuffer;
    // 捕获为Epoll事件数组
    epoll_event* events = pStu->pEvents;
    // epoll集合，用以增加、删除客户端
    int epollfd = *pStu->pfdEpoll;
    // 服务器Socket，用以判断新连接产生
    int listenfd = *pStu->pfdListen;
    // 线程池指针
    CThreadPoolManager* pMgrThreadPool = (CThreadPoolManager*)pStu->pMgrThreadPool;

    for (int i = 0; i < number; i++)
    {
        int sockfd = events[i].data.fd;

        if (sockfd == listenfd)
        {
            struct sockaddr_in client_address;
            socklen_t client_addresslen = sizeof(client_address);
            int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addresslen);
            Addfd(&client_address, pMapBuffer, epollfd, connfd);
            LOG("Client is Come In...\n");

        }
        else if (events[i].events & EPOLLIN)
        {
            while (true)
            {
                memset(buf, 0x00, sizeof(buf));
                int ret = recv(sockfd, buf, SIZE_PACKET_MAX - 1, 0);

                if (ret < 0)
                {
                    if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                    {
                        LOG("Recv Completed.\n");
                        break;
                    }
                    Delfd(pMapBuffer, epollfd, sockfd);
                    break;
                }
                else if (0 == ret)
                {
                    Delfd(pMapBuffer, epollfd, sockfd);
                    break;
                }
                else
                {
                    pPacket = new CSocketPacket;
                    // 放入环形缓冲区，然后再从环形缓冲区中弹出完整包
                    (*pMapBuffer)[sockfd]->GetBuffer()->PushByte(buf, ret);
                    while ((*pMapBuffer)[sockfd]->GetBuffer()->PopPacket(pPacket->setTotalData()))
                    {
                        if (pPacket->isChecked())
                        {
                            pMgrThreadPool->Run(MyProcFunc, (void*)pPacket);
                            pPacket = new CSocketPacket;
                        }
                        else
                        {
                            LOG("Pack is not Check.\n");
                        }
                    }
                    delete pPacket;
                }
            }
        }
        else
        {
            LOG("something else happen.\n");
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::ThreadEpollWork
/// 功   能：Epoll轮询线程函数，阻塞在epoll_wait函数上
/// 参   数：void* param               类中需要传入线程中的成员变量
/// 返回 值：void*                      private
////////////////////////////////////////////////////////////////////////////////
void* TCPSocket::ThreadEpollWork(void* param)
{
    pthread_detach(pthread_self());
    stuThreadParam* pStu = (stuThreadParam*)param;
    while (*pStu->pOnOff)
    {
        usleep(1000 * 20);
        int ret = epoll_wait(*pStu->pfdEpoll, pStu->pEvents, *pStu->pRLimitNum, -1);
        if (ret < 0)
        {
            LOG("Epoll wait Error....");
        }
        //EpollELWork((map<int, CSocketClient*>*)pStu->pMapBuffer, pStu->pEvents, ret, *pStu->pfdEpoll, *pStu->pfdListen);
        EpollELWork(pStu, ret);
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::LoadConfig
/// 功   能：读取配置文件
/// 参   数：void
/// 返回 值：bool           private
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::LoadConfig()
{
    CConfig::getInstance()->GetConfigStringValue(CONFIG_PATH, "Server", "IP", m_strCfgIP);
    CConfig::getInstance()->GetConfigIntValue(CONFIG_PATH, "Server", "PORT", &m_nCfgPort);
    CConfig::getInstance()->GetConfigIntValue(CONFIG_PATH, "Server", "RLIMIT_NUM", &m_nCfgLimitNum);

    CConfig::getInstance()->GetConfigIntValue(CONFIG_PATH, "Thread Pool", "SIZE_TASK", &m_nSizeTask);
    CConfig::getInstance()->GetConfigIntValue(CONFIG_PATH, "Thread Pool", "SIZE_THREAD_MAX", &m_nSizeThreadPoolMax);
    CConfig::getInstance()->GetConfigIntValue(CONFIG_PATH, "Thread Pool", "SIZE_THREAD_PRE", &m_nSizeThreadPoolPre);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::SetLimitMax
/// 功   能：设置文件连接最大数
/// 参   数：void
/// 返回 值：bool           private
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::SetLimitMax()
{
    m_rt.rlim_max = m_rt.rlim_cur = m_nCfgLimitNum;
    if (setrlimit(RLIMIT_NOFILE, &m_rt) == -1)
    {
        if(1 == errno)
        {
            LOG("Set rlimit is Error.but continue.\n");
            return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::ShowSocketStatus
/// 功   能：显示机器属性
/// 参   数：void
/// 返回 值：bool           private
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::ShowSocketStatus(void)
{
    CTimeVal tm1, tm2, tm3;
    struct rlimit tmpRt;
    memset(&tmpRt, 0, sizeof(rlimit));
    int a = 0x0001;
    char* p = (char*)&a;
    LOG("[CMAIN]==========Show Test Data==========\n");
    LOG("[CMAIN]bool Size:%d.\n", sizeof(bool));
    LOG("[CMAIN]short Size:%d.\n", sizeof(short));
    LOG("[CMAIN]int Size:%d.\n", sizeof(int));
    LOG("[CMAIN]long Size:%d.\n", sizeof(long));
    LOG("[CMAIN]float Size:%d.\n", sizeof(float));
    LOG("[CMAIN]double Size:%d.\n", sizeof(double));
    if(*p)
    {
        LOG("[CMAIN]Endian:Little Endian.*p=%d.\n", (int)*p);
    }
    else
    {
        LOG("[CMAIN]Endian:Big-Endian.*p=%d.\n", (int)*p);
    }
    tm1.getHereTime();
    for(a = 0; a < 1000000; a++);
    tm2.getHereTime();
    tm3 = tm2 - tm1;
    LOG("[CMAIN]for(a = 0; a < 1000000; a++);a=%d\n", a);
    LOG("[CMAIN]tm1 sec:%ld, usec:%ld.\n", tm1.getHereSec(), tm1.getHereUSec());
    LOG("[CMAIN]tm2 sec:%ld, usec:%ld.\n", tm2.getHereSec(), tm2.getHereUSec());
    LOG("[CMAIN]Diff MS:%ld.\tDiff US:%ld.\n", tm3.getHereTotalMSec(), tm3.getHereTotalUSec());
    LOG("[CMAIN]TCP Server Port:%d.\n", m_nCfgPort);
    if(-1 == getrlimit(RLIMIT_NOFILE, &tmpRt))
    {
        LOG("Get rlimit is error.\n");
        return false;
    }
    m_nCfgLimitNum = tmpRt.rlim_cur;
    LOG("[CSocket]get rlimit Cur:%d. Max:%d.\n", tmpRt.rlim_cur, tmpRt.rlim_max);
    LOG("[CMAIN]ThreadPool Task:%d PoolMax:%d PoolPre:%d.\n", m_nSizeTask, m_nSizeThreadPoolMax, m_nSizeThreadPoolPre);
    LOG("[CMAIN]Build by 2015.12.07 19:03.\n");
    LOG("[CMAIN]=============Test End============\n");

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::MaskSignal
/// 功   能：屏蔽忽略SIGPIPE信号错误
/// 参   数：void
/// 返回 值：bool                       private
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::ThreadPollInit()
{
    if(0 > m_mgrThreadPool.Init(m_nSizeTask,  m_nSizeThreadPoolMax, m_nSizeThreadPoolPre))
    {
        cout << "初始化失败" << endl;
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::MaskSignal
/// 功   能：屏蔽忽略SIGPIPE信号错误
/// 参   数：void
/// 返回 值：bool                       private
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::MaskSignal()
{
    sigset_t signal_mask;
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGPIPE);
    int rc = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
    if (0 != rc)
    {
        LOG("mask signal pipe error.\n");
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::CreateSocket
/// 功   能：创建服务器Socket对象
/// 参   数：void
/// 返回 值：bool                       private
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::CreateSocket()
{
    m_fdListen = socket(AF_INET, SOCK_STREAM, 0);
    if (m_fdListen < 0)
    {
        LOG("Create Socket is Error.\n");
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::CreateEpoll
/// 功   能：创建Epoll对象
/// 参   数：void
/// 返回 值：bool                       private
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::CreateEpoll()
{
    m_fdEpoll = epoll_create(5);
    if (m_fdEpoll < 0)
    {
        LOG("Create Epoll is Error.\n");
        return false;
    }
    Addfd(&m_addr, (map<int, CSocketClient*>*)&m_mapBuffer, m_fdEpoll, m_fdListen);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::BindSocket
/// 功   能：绑定Socket
/// 参   数：void
/// 返回 值：bool                       private
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::BindSocket()
{
    bzero(&m_addr, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    inet_pton(AF_INET, m_strCfgIP, &m_addr.sin_addr);
    m_addr.sin_port = htons(m_nCfgPort);

    int nRet = bind(m_fdListen, (struct sockaddr*)&m_addr, sizeof(m_addr));
    if (nRet < 0)
    {
        LOG("Bind Socket is Error.\n");
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::ListenSocket
/// 功   能：开启服务器Socket的监听
/// 参   数：void
/// 返回 值：bool                       private
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::ListenSocket()
{
    int nRet = listen(m_fdListen, 5);
    if (nRet < 0)
    {
        LOG("Listen Socket is Error.\n");
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::Init
/// 功   能：初始化对象
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::Init()
{
    // 读取配置文件
    if (!LoadConfig())
    {
        return false;
    }
    // 设置读写ID口文件最大数
    SetLimitMax();
    // 显示当前Socket配置情况
    if (!ShowSocketStatus())
    {
        return false;
    }
    // 初始化线程池
    if (!ThreadPollInit())
    {
        return false;
    }
    // 屏蔽多重发送的报错信号
    if (!MaskSignal())
    {
        return false;
    }
    // 创建Socket
    if (!CreateSocket())
    {
        return false;
    }
    // 创建Epoll
    if (!CreateEpoll())
    {
        return false;
    }
    // 绑定Socket
    if (!BindSocket())
    {
        return false;
    }
    // 开启监听该Socket
    if (!ListenSocket())
    {
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::Run
/// 功   能：开启Epoll工作线程
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::Run()
{
    pthread_t ptdEpollWork;
    m_bThreadEpollWork = true;
    pthread_create(&ptdEpollWork, NULL, ThreadEpollWork, &m_parWork);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：TCPSocket::Destory
/// 功   能：销毁对象
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::Destory()
{
    m_bThreadEpollWork = false;
    // delete all socket

    return true;
}
