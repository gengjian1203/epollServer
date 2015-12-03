#include "ThreadPool.h"

CThreadPool::CThreadPool()
    : m_poolMax(0)
    , m_idleNum(0)
    , m_totalNum(0)
    , m_bStop(false)
{
    pthread_mutex_init(&m_mutex, NULL);
    pthread_mutex_init(&m_runMutex,NULL);
    pthread_mutex_init(&m_terminalMutex, NULL);
    pthread_cond_init(&m_terminalCond, NULL);
    pthread_cond_init(&m_emptyCond, NULL);
}

CThreadPool::~CThreadPool()
{
    /*if(!m_threads.empty())
    {
        std::list<Thread*>::iterator it = m_threads.begin();
        for(; it != m_threads.end(); ++it)
        {
            if(*it != NULL)
            {
                pthread_cond_destroy( &((*it)->m_cond) );
                pthread_mutex_destroy( &((*it)->m_mutex) );
                delete *it;
                *it = NULL;
            }
        }
        m_threads.clear();
    }*/
    pthread_mutex_destroy(&m_runMutex);
    pthread_mutex_destroy(&m_terminalMutex);
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_terminalCond);
    pthread_cond_destroy(&m_emptyCond);
}

int CThreadPool::InitPool(const int & poolMax, const int & poolPre)
{
    if(poolMax < poolPre
            || poolPre < 0
            || poolMax <= 0)
    {
        return -1;
    }
    m_poolMax = poolMax;

    int iRet = 0;
    for(int i=0; i<poolPre; ++i)
    {
        CThread * thread = CreateThread();
        if(NULL == thread)
        {
            iRet = -2;
        }
    }

    if(iRet < 0)
    {
        std::list<CThread*>::iterator it = m_threads.begin();
        for(; it!= m_threads.end(); ++it)
        {
            if(NULL != (*it) )
            {
                delete *it;
                *it = NULL;
            }
        }
        m_threads.clear();
        m_totalNum = 0;
    }
    return iRet;
}

void CThreadPool::GetThreadRun(task_fun fun, void* arg)
{
    //从线程池中获取一个线程
    pthread_mutex_lock( &m_mutex);
    if(m_threads.empty())
    {
        pthread_cond_wait(&m_emptyCond,&m_mutex); //阻塞等待有空闲线程
    }

    CThread * thread = m_threads.front();
    m_threads.pop_front();
    pthread_mutex_unlock( &m_mutex);

    pthread_mutex_lock( &thread->m_mutex );
    thread->task.fun = fun;
    thread->task.data = arg;
    pthread_cond_signal(&thread->m_cond); //触发线程WapperFun循环执行
    pthread_mutex_unlock( &thread->m_mutex );
}

int CThreadPool::Run(task_fun fun, void * arg)
{
    pthread_mutex_lock(&m_runMutex); //保证每次只能由一个线程执行
    int iRet = 0;
    if(m_totalNum <m_poolMax) //
    {
        if(m_threads.empty() && (NULL == CreateThread()) )
        {
            iRet = -1;//can not create new thread!
        }
        else
        {
            GetThreadRun(fun, arg);
        }
    }
    else
    {
        GetThreadRun(fun, arg);
    }
    pthread_mutex_unlock(&m_runMutex);
    return iRet;
}

void CThreadPool::StopPool(bool bStop)
{
    m_bStop = bStop;
    if(bStop)
    {
        //启动监控所有空闲线程是否退出的线程
        CThread thread(false, this);
        pthread_create(&thread.m_threadId,&thread.m_attr, CThreadPool::TerminalCheck , &thread); //启动监控所有线程退出线程
        //阻塞等待所有空闲线程退出
        pthread_join(thread.m_threadId, NULL);
    }
    /*if(bStop)
    {
        pthread_mutex_lock(&m_terminalMutex);
        //启动监控所有空闲线程是否退出的线程
        Thread thread(true, this);
        pthread_create(&thread.m_threadId,&thread.m_attr, ThreadPool::TerminalCheck , &thread); //启动监控所有线程退出线程
        //阻塞等待所有空闲线程退出
        pthread_cond_wait(&m_terminalCond, & m_terminalMutex);
        pthread_mutex_unlock(&m_terminalMutex);
    }*/
}

bool CThreadPool::GetStop()
{
    return m_bStop;
}

CThread * CThreadPool::CreateThread()
{
    CThread * thread = NULL;
    thread = new CThread(true, this);
    if(NULL != thread)
    {
        int iret = pthread_create(&thread->m_threadId,&thread->m_attr, CThreadPool::WapperFun , thread); //通过WapperFun将线程加入到空闲队列中
        if(0 != iret)
        {
            delete thread;
            thread = NULL;
        }
    }
    return thread;
}

void* CThreadPool::WapperFun(void*arg)
{
    CThread * thread = (CThread*)arg;
    if(NULL == thread || NULL == thread->m_pool)
    {
        return NULL;
    }
    CThreadPool * pool = thread->m_pool;
    pool->IncreaseTotalNum();
    struct timespec abstime;
    memset(&abstime, 0, sizeof(abstime));
    while(1)
    {
        if(0 != thread->task.fun)
        {
            thread->task.fun(thread->task.data);
        }

        if( true == pool->GetStop() )
        {
            break; //确定当前任务执行完毕后再判定是否退出线程
        }
        pthread_mutex_lock( &thread->m_mutex );
        pool->SaveIdleThread(thread); //将线程加入到空闲队列中
        abstime.tv_sec = time(0) + THREAD_WAIT_TIME_OUT;
        abstime.tv_nsec = 0;
        if(ETIMEDOUT  == pthread_cond_timedwait( &thread->m_cond, &thread->m_mutex, &abstime )) //等待线程被唤醒 或超时自动退出
        {
            pthread_mutex_unlock( &thread->m_mutex );
            break;
        }
        pthread_mutex_unlock( &thread->m_mutex );
    }

    pool->LockMutex();
    pool->DecreaseTotalNum();
    if(thread != NULL)
    {
        pool->RemoveThread(thread);
        delete thread;
        thread = NULL;
    }
    pool->UnlockMutex();
    return 0;
}

void CThreadPool::SaveIdleThread(CThread * thread)
{
    if(thread)
    {
        thread->task.fun = 0;
        thread->task.data = NULL;
        LockMutex();
        if(m_threads.empty())
        {
            pthread_cond_broadcast(&m_emptyCond); //发送不空的信号,告诉run函数线程队列已经不空了
        }
        m_threads.push_front(thread);
        UnlockMutex();
    }
}

int CThreadPool::TotalThreads()
{
    return m_totalNum;
}


void CThreadPool::SendSignal()
{
    LockMutex();
    std::list<CThread*>::iterator it = m_threads.begin();
    for(; it!= m_threads.end(); ++it)
    {
        pthread_mutex_lock( &(*it)->m_mutex );
        pthread_cond_signal(&((*it)->m_cond));
        pthread_mutex_unlock( &(*it)->m_mutex );
    }
    UnlockMutex();
}

void* CThreadPool::TerminalCheck(void* arg)
{
    CThread * thread = (CThread*)arg;
    if(NULL == thread || NULL == thread->m_pool)
    {
        return NULL;
    }
    CThreadPool * pool = thread->m_pool;
    while((false == pool->GetStop()) || pool->TotalThreads() >0 )
    {
        pool->SendSignal();

        usleep(IDLE_CHECK_POLL_EMPTY);
    }
    //pool->TerminalCondSignal();
    return 0;
}

void CThreadPool::TerminalCondSignal()
{
    pthread_cond_signal(&m_terminalCond);
}

void CThreadPool::RemoveThread(CThread* thread)
{
    m_threads.remove(thread);
}

void CThreadPool::LockMutex()
{
    pthread_mutex_lock( &m_mutex);
}

void CThreadPool::UnlockMutex()
{
    pthread_mutex_unlock( &m_mutex );
}

void CThreadPool::IncreaseTotalNum()
{
    LockMutex();
    m_totalNum++;
    UnlockMutex();
}
void CThreadPool::DecreaseTotalNum()
{
    m_totalNum--;
}
