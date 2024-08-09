#ifndef CANSEND_H
#define CANSEND_H

#include <QDebug>
#include "zlgcan.h"
#include <QString>

class canSender
{
public:
    void Send(CHANNEL_HANDLE channel_handle,uint32_t id,const uint8_t *data,uint8_t len);
};

#endif // CANSEND_H
