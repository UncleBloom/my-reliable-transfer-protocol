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
private:
    int base;                            //基序号
    int windowSize;                      //窗口长度
    std::map<int, Packet> slidingWindow;//接收方的滑动窗口

    bool inline inWindow(int rcvSeqNum) const;//接收到的序列号是否在窗口内


public:
    //接收报文，将被NetworkService调用
    void receive(const Packet &packet);

    TCPReceiver();
};

#endif//MYRDT_TCPRECEIVER_H
