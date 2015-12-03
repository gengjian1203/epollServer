#ifndef __GLOBAL_H__
#define __GLOBAL_H__

// 系统
#include <stdio.h>                  // I/O
#include <iostream>                 // I/O
#include <stdlib.h>                 // malloc free
#include <stdarg.h>                 // 变参
#include <memory.h>                 // 内存管理
#include <signal.h>                 // signal warning error.
#include <unistd.h>                 // usleep
#include <errno.h>                  // errno
#include <netinet/in.h>             // sockaddr_in
#include <arpa/inet.h>              // inet_addr
#include <time.h>
#include <sys/time.h>               // 时间
#include <sys/resource.h>           // *setrlimit */
#include <sys/epoll.h>              // epoll function
#include <fcntl.h>
#include <queue>                    // STL queue队列
#include <map>                      // STL map
#include <list>
#include <pthread.h>
#include <mysql/mysql.h>

using namespace std;


//#define CONFIG_PATH                 "/home/gj/github/epollServer/Document/Config.ini"
#define CONFIG_PATH                 "./Document/Config.ini"
#define RLIMIT_MAX                  (65525)
#define GLODEN_KEY                  (273565573)
#define SIZE_PACKET_MAX             (1024)
#define SIZE_STRSQL_MAX             (512)
#define SIZE_CLIENT_DATE_MAX        (20)

#define SERVER_CMD_TEST             (99)

//单位秒，监测空闲列表时间间隔，在空闲队列中超过TASK_DESTROY_INTERVAL时间的任务将被自动销毁
const int CHECK_IDLE_TASK_INTERVAL = 300;
//单位秒，任务自动销毁时间间隔
const int TASK_DESTROY_INTERVAL = 60;
//监控线程池是否为空时间间隔,微秒
const int IDLE_CHECK_POLL_EMPTY = 500;
//线程池线程空闲自动退出时间间隔 ,5分钟
const int  THREAD_WAIT_TIME_OUT = 300;

#define __DEBUG

#ifdef __DEBUG
#define LOG(format, args...)              printf((format), ##args)
#else
#define LOG(format, args...)
#endif

struct stuThreadParam
{
    int* pfdListen;
    int* pfdEpoll;
    epoll_event* pEvents;
    map<int, void*>* pMapBuffer;
    int* pRLimitNum;

    bool* pOnOff;
};

#endif // GLOBAL_H
