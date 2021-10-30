//
// Created by 王一舟 on 2021/10/28.
//

#ifndef MYRDT_TCPRECEIVER_H
#define MYRDT_TCPRECEIVER_H

#include "Global.h"
#include "RdtReceiver.h"
#include <map>

#define WINDOW_SIZE 6

class TCPReceiver : public RdtReceiver {

public:
    //接收报文，将被NetworkService调用
    void receive(const Packet &packet);

    TCPReceiver();
};

#endif//MYRDT_TCPRECEIVER_H
