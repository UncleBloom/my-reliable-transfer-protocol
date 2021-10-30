//
// Created by 王一舟 on 2021/10/28.
//

#ifndef MYRDT_TCPSENDER_H
#define MYRDT_TCPSENDER_H

#include "Global.h"
#include "RdtSender.h"
#include <list>

#define WINDOW_SIZE 6


class TCPSender : public RdtSender {
public:
    bool send(const Message &message); //发送应用层下来的Message，由NetworkService调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待确认状态或发送窗口已满而拒绝发送Message，则返回false
    void receive(const Packet &ackPkt);//接受确认Ack，将被NetworkService调用
    void timeoutHandler(int seqNum);   //Timeout handler，将被NetworkService调用
    bool inline getWaitingState();     //返回RdtSender是否处于等待状态，如果发送方正等待确认或者发送窗口已满，返回true

    TCPSender();
};

#endif//MYRDT_TCPSENDER_H
