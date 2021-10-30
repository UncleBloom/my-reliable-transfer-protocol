//
// Created by 王一舟 on 2021/10/23.
//

#ifndef MYRDT_GBNRECEIVER_H
#define MYRDT_GBNRECEIVER_H

#include "RdtReceiver.h"

class GBNReceiver : public RdtReceiver {
private:
    //期待收到的下一个报文序号
    int expectSeqNum;

    //上次发送的确认报文
    Packet lastAckPkt;

public:
    GBNReceiver();
    ~GBNReceiver() override = default;

    //接收报文，将被NetworkService调用
    void receive(const Packet &packet);
};

#endif//MYRDT_GBNRECEIVER_H
