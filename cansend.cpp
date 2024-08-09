#include "cansend.h"

void canSender::Send(CHANNEL_HANDLE channel_handle,uint32_t id,const uint8_t *data,uint8_t len)
{
    ZCAN_Transmit_Data frame;
    memset(&frame, 0, sizeof(frame));
    frame.frame.can_id = MAKE_CAN_ID(id, 0, 0, 0);
    frame.frame.can_dlc = len;
    frame.transmit_type = 0;
    memcpy(frame.frame.data, data, len);
    if (ZCAN_Transmit(channel_handle, &frame, 1) != 1)
    {
        qDebug()  << "发送数据失败";
    }
    return;
}



