#ifndef __SQL_MANAGER_H__
#define __SQL_MANAGER_H__

#include "LockAuto.h"

class CSQLManager
{
private:
    CSQLManager();
    ~CSQLManager();

private:
    bool m_bisConnect;
    MYSQL m_sqlConnection;
    CLock m_lock;

public:
    static CSQLManager* getInstance();

    bool isConnect();

    bool Init();
    bool Destory();

    bool Query(char* pStrSQL);

};

#endif // SQLMANAGER_H
