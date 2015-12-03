#ifndef __CTHREAD_POOL_H__
#define __CTHREAD_POOL_H__

#include "Thread.h"

class CThreadPool
{
public:
    CThreadPool();
    ~CThreadPool();

    /* pur @ 初始化线程池
     * para @ poolMax 线程池最大线程数
     * para @ poolPre 预创建线程数
     * return @ 0:成功
     *          -1: parameter error, must poolMax > poolPre >=0
     *          -2: 创建线程失败
    */
    int InitPool(const int & poolMax, const int & poolPre);

    /* pur @ 执行一个任务
     * para @ task 任务指针
     * return @ 0任务分配成功，负值 任务分配失败，-1，创建新线程失败
    */
    int Run(task_fun fun, void* arg);

    /* pur @ 设置是否停止线程池工作
     * para @ bStop true停止，false不停止
    */
    void StopPool(bool bStop);

public: //此公有函数主要用于静态函数调用
    /* pur @ 获取进程池的启停状态
     * return @
    */
    bool GetStop();
    void SaveIdleThread(CThread * thread );
    void LockMutex();
    void UnlockMutex();
    void DecreaseTotalNum();
    void IncreaseTotalNum();
    void RemoveThread(CThread* thread);
    void TerminalCondSignal();
    int TotalThreads();
    void SendSignal();
private:
    /* pur @ 创建线程
     * return @ 非空 成功，NULL失败，
    */
    CThread * CreateThread();

    /* pur @ 从线程池中获取一个一个线程运行任务
     * para @ fun 函数指针
     * para @ arg 函数参数
     * return @
    */
    void GetThreadRun(task_fun fun, void* arg);

    static void * WapperFun(void*);
    static void * TerminalCheck(void*);//循环监测是否所有线程终止线程

private:
    int m_poolMax;//线程池最大线程数
    int m_idleNum; //空闲线程数
    int m_totalNum; //当前线程总数 小于最大线程数
    bool m_bStop; //是否停止线程池
    pthread_mutex_t m_mutex; //线程列表锁
    pthread_mutex_t m_runMutex; //run函数锁

    pthread_mutex_t m_terminalMutex; //终止所有线程互斥量
    pthread_cond_t  m_terminalCond; //终止所有线程条件变量
    pthread_cond_t  m_emptyCond; //空闲线程不空条件变量

    std::list<CThread*> m_threads; // 线程列表
};

#endif // THREADPOOL_H
