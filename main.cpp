#include <stdio.h>
#include "TcpSocket.h"
#include "SqlManager.h"

CSocketBuffer _buffer;

void* ThreadProducer(void* param)
{
    pthread_detach(pthread_self());
    CSocketPacket pack;
    int nR = 0;
    while (true)
    {
        usleep(1000 * 20);
        nR = rand() % 100;
        pack.formatData();
        pack.set_CMD(121);
        pack.push_bool(true);
        pack.push_int(nR);
        if(nR < 10)
        {
            pack.push_byteArray("aaa111", 7);
        }
        else if (nR < 20)
        {
            pack.push_byteArray("bbbb2222", 9);
        }
        else if (nR < 30)
        {
            pack.push_byteArray("ccccc33333", 11);
        }
        else if (nR < 40)
        {
            pack.push_byteArray("dddddd444444", 13);
        }
        else if (nR < 50)
        {
            pack.push_byteArray("eeeeeee5555555", 15);
        }
        else
        {
            pack.push_byteArray("ffffffffffffffffffff", 21);
        }
        pack.push_finish();
        if (pack.isCompleted())
        {
            if (!_buffer.PushByte(pack.getTotalData(), pack.get_Length()))
            {
                LOG("buffer is not push.\n");
            }
        }
        else
        {
            LOG("packet is not completed.\n");
        }
    }
    return 0;
}

void* ThreadConsumer(void* param)
{
    pthread_detach(pthread_self());
    CSocketPacket pack;
    int nCMD = 0;
    bool nBool = false;
    int nInt = 0;
    char* pStr = 0x00;
    int nStrLen = 0;

    while (true)
    {
        usleep(1000 * 20);
        if (_buffer.PopPacket(pack.setTotalData()))
        {
            if (pack.isChecked())
            {
                nCMD = pack.get_CMD();
                nBool = pack.pop_bool();
                nInt = pack.pop_int();
                pack.pop_byteArray(pStr, nStrLen);
                LOG("Cons|CMD:%d,Bool:%d,Int:%d,Str:%s,Len:%d.\n", nCMD, nBool, nInt, pStr, nStrLen);
            }
            else
            {
                LOG("packet is not checked.\n");
            }
        }
        else
        {
            LOG("buffer is not pop.\n");
        }

    }
    return 0;
}

bool StartTest()
{
    //  buffer test
    for (int i = 0; i < 3; i++)
    {
        pthread_t ptdProducer;
        pthread_create(&ptdProducer, NULL, ThreadProducer, NULL);
    }

    for (int j = 0; j < 2; j++)
    {
        pthread_t ptdConsumer;
        pthread_create(&ptdConsumer, NULL, ThreadConsumer, NULL);
    }
    return true;

}

int main()
{

    TCPSocket tcp;
    printf("hello epoll server.\n");

    CSQLManager::getInstance()->Init();
    tcp.Init();

    LOG("TCP Init done.\n");

    tcp.Run();

    getchar();

    tcp.Destory();
    CSQLManager::getInstance()->Destory();


    //StartTest();



    getchar();
    return 0;
}
