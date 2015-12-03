#include "SqlManager.h"

CSQLManager::CSQLManager()
    : m_bisConnect(false)
{

}

CSQLManager::~CSQLManager()
{

}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSQLManager::getInstance
/// 功   能：获取数据库管理者单例指针
/// 参   数：void
/// 返回 值：CSQLManager*                public
////////////////////////////////////////////////////////////////////////////////
CSQLManager* CSQLManager::getInstance()
{
    static CSQLManager instance;
    return &instance;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSQLManager::isConnect
/// 功   能：获取数据库管理者是否连接上服务器
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSQLManager::isConnect()
{

    return m_bisConnect;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSQLManager::Init
/// 功   能：初始化对象
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSQLManager::Init()
{
    // init
    mysql_init(&m_sqlConnection);
    // connect datebases
    if (mysql_real_connect(&m_sqlConnection, "localhost", "root", "admin", "testsql", 0, NULL, CLIENT_FOUND_ROWS))
    {
        LOG("Connect MySQL Success.\n");
        m_bisConnect = true;
    }
    else
    {
        LOG("Connect MySQL Failed.\n");
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSQLManager::Destory
/// 功   能：销毁对象
/// 参   数：void
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSQLManager::Destory()
{
    if (m_bisConnect)
    {
        mysql_close(&m_sqlConnection);
        m_bisConnect = false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// 名   称：CSQLManager::Query
/// 功   能：执行指定SQL语句（插入、删除）
/// 参   数：char* pStrSQL             SQL语句存放的字符串指针
/// 返回 值：bool                       public
////////////////////////////////////////////////////////////////////////////////
bool CSQLManager::Query(char* pStrSQL)
{
    CLockAuto autolock(m_lock);
    int nRet = 0;

    if (m_bisConnect)
    {
        nRet = mysql_query(&m_sqlConnection, pStrSQL);
        if (0 == nRet)
        {
            LOG("Query is Success.\n");
            return true;
        }
        else
        {
            LOG("Query is Error %d: %s.\n", mysql_errno(&m_sqlConnection), mysql_error(&m_sqlConnection));
        }
    }

    return false;
}
