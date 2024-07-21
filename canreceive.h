#ifndef CANRECEIVE_H
#define CANRECEIVE_H

#include <QObject>
#include <QDebug>
#include <QVector>
#include <QThread>
#include "zlgcan.h"

class CANReceive : public QThread
{
    Q_OBJECT
public:
    explicit CANReceive(QObject *parent = nullptr);

protected:
    void run();

signals:
    //自定义信号，传递数据
    void curNumber(int num);

public slots:

private:
    CHANNEL_HANDLE handle;

public:
    void setHandle( CHANNEL_HANDLE h);
};


#endif //
