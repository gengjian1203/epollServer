#ifndef __CTHREAD_H__
#define __CTHREAD_H__

#include "TaskPool.h"

class CThreadPool;

class CThread
{
public:
    CThread(bool detach, CThreadPool * pool);
    ~CThread();
    pthread_t  m_threadId; //线程id
    pthread_mutex_t m_mutex; //互斥锁
    pthread_cond_t m_cond; //条件变量
    pthread_attr_t m_attr; //线程属性
    Task  task; //
    CThreadPool * m_pool; //所属线程池
};

#endif // THREAD_H
