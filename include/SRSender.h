//
// Created by 王一舟 on 2021/10/27.
//

#ifndef MYRDT_SRSENDER_H
#define MYRDT_SRSENDER_H

#include "RdtSender.h"
#include "Global.h"
#include <list>

#define WINDOW_SIZE 6

typedef int pktStatus;

//包的状态
enum {
    sent,    //发送，还未确认
    confirmed//已确认
};

class SRSender : public RdtSender {
private:
    int base;                                       //基序号
    int windowSize;                                 //窗口长度
    int nextSeqNum;                                 //下一个待发送包的序号
    std::list<std::pair<pktStatus, Packet>> slidingWindow;//窗口内还未发送的包

    bool inline inWindow(int rcvSeqNum) const;//接收到的确认号是否在窗口内

public:
    bool send(const Message &message); //发送应用层下来的Message，由NetworkService调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待确认状态或发送窗口已满而拒绝发送Message，则返回false
    void receive(const Packet &ackPkt);//接受确认Ack，将被NetworkService调用
    void timeoutHandler(int seqNum);   //Timeout handler，将被NetworkService调用
    bool inline getWaitingState();     //返回RdtSender是否处于等待状态，如果发送方正等待确认或者发送窗口已满，返回true

    SRSender();
};

#endif//MYRDT_SRSENDER_H
