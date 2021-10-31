//
// Created by 王一舟 on 2021/10/27.
//

#ifndef MYRDT_SRRECEIVER_H
#define MYRDT_SRRECEIVER_H

#include "RdtReceiver.h"
#include "Global.h"
#include <map>

#define WINDOW_SIZE 6

class SRReceiver : public RdtReceiver {
private:
    int base;                            //基序号
    int windowSize;                      //窗口长度
    std::map<int, Packet> slidingWindow;//接收方的滑动窗口

    bool inline inWindow(int rcvSeqNum) const;//接收到的序列号是否在窗口内

public:
    SRReceiver();

    //接收报文，将被NetworkService调用
    void receive(const Packet &packet);
};

#endif//MYRDT_SRRECEIVER_H
