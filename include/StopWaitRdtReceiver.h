//
// Created by 王一舟 on 2021/10/22.
//

#ifndef STOPWAIT_STOPWAITRDTRECEIVER_H
#define STOPWAIT_STOPWAITRDTRECEIVER_H

#include "RdtReceiver.h"
class StopWaitRdtReceiver : public RdtReceiver {
private:
    int expectSequenceNumberRcvd;// 期待收到的下一个报文序号
    Packet lastAckPkt;           //上次发送的确认报文

public:
    StopWaitRdtReceiver();
    virtual ~StopWaitRdtReceiver();

public:
    void receive(const Packet &packet);//接收报文，将被NetworkService调用
};

#endif//STOPWAIT_STOPWAITRDTRECEIVER_H
