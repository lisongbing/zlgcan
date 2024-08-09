#include <QThread>

#include "canreceive.h"
#include <QDebug>
#include "zlgcan.h"
#include <QString>

CANReceive::CANReceive(QObject *parent) : QThread(parent){}

void CANReceive::setHandle( CHANNEL_HANDLE h)
{
    this->handle = h;
}
void CANReceive::run()
{
    qDebug() << "当前线程对象的地址: " << QThread::currentThread();

    ZCAN_Receive_Data data[100] = {};
    while (1)
    {
        int count = ZCAN_GetReceiveNum(handle, 0); // 获取 CAN 报文（参数 2：0 - CAN，1 - CANFD）
    //数量
    while (count > 0)
    {
    int rcount = ZCAN_Receive(handle, data, 100, 10);
    for (int i = 0; i < rcount; ++i)
    {
        QString result = QString::asprintf("%x",((data[i].frame.can_id) & 0x1FFFFFFF));
        qDebug()  << result;
        QByteArray byteArray;
        for (int j = 0;j < 8;j++)
        {
            byteArray.append(data[i].frame.data[j]);
        }

        qDebug()  << "id:" + byteArray.toHex();
    }

    count -= rcount;
    }
    //106

        QThread::usleep(1);
    }
   // std::cout << "chnl: " << std::dec << ch << " thread exit" << std::endl;
    qDebug() << "run() 执行完毕, 子线程退出...";
}
