#include "Thread.h"

CThread::CThread(bool detach, CThreadPool * pool)
    : m_pool(pool)
{
    pthread_attr_init(&m_attr);
    if(detach)
    {
        pthread_attr_setdetachstate(&m_attr, PTHREAD_CREATE_DETACHED ); // 让线程独立运行
    }
    else
    {
         pthread_attr_setdetachstate(&m_attr, PTHREAD_CREATE_JOINABLE );
    }

    pthread_mutex_init(&m_mutex, NULL); //初始化互斥量
    pthread_cond_init(&m_cond, NULL); //初始化条件变量
    task.fun = 0;
    task.data = NULL;
}

CThread::~CThread()
{
    pthread_cond_destroy(&m_cond);
    pthread_mutex_destroy(&m_mutex);
    pthread_attr_destroy(&m_attr);
}
