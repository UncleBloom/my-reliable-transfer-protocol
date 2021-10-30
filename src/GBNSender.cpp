//
// Created by 王一舟 on 2021/10/23.
//
#include "../include/GBNSender.h"
#include "../include/Global.h"

bool inline GBNSender::inWindow(int rcvSeqNum) const {
    return rcvSeqNum >= base && rcvSeqNum <= (base + windowSize - 1);
}

GBNSender::GBNSender() : base(0), windowSize(WINDOW_SIZE), nextSeqNum(0) {}

/*
 * 若发送窗口已满则返回true
 */
bool inline GBNSender::getWaitingState() {
    return nextSeqNum > (base + windowSize - 1);
}

/**
 * @description 发送应用层的Message，由NetworkService调用
 * @return 如果成功地将Message发送到网络层，返回true;如果发送窗口已满而拒绝发送Message，则返回false
 */
bool GBNSender::send(const Message &message) {
    //发送窗口已满
    if (getWaitingState()) { return false; }

    Packet packet;
    packet.acknum = -1;//忽略该字段
    packet.seqnum = this->nextSeqNum;
    memcpy(packet.payload, message.data, sizeof(message.data));
    packet.checksum = pUtils->calculateCheckSum(packet);

    //将包放入滑动窗口，并发送报文
    slidingWindow.push_back(packet);
    pUtils->printPacket("S-- sendPkt: ", packet);
    //如果是窗口中的第一个包则启动发送方定时器
    if (this->base == this->nextSeqNum) {
        pns->startTimer(SENDER, Configuration::TIME_OUT, nextSeqNum);
    }
    pns->sendToNetworkLayer(RECEIVER, packet);

    nextSeqNum++;
    return true;
}

/**
 * @description 接受确认Ack，将被NetworkService调用
 * @param ackPkt 接受的ACK包
 */
void GBNSender::receive(const Packet &ackPkt) {
    //检查校验和
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    int ackNum = ackPkt.acknum;
    if (checkSum == ackPkt.checksum && inWindow(ackNum)) {
        //如果校验和正确，并且确认号在窗口内
        pUtils->printPacket("S-- RcvAck: ", ackPkt);
        pns->stopTimer(SENDER, this->base);
        while (ackNum >= base) {
            pUtils->printPacket(("$ Slid window from " + to_string(this->base) + ": ").data(), this->slidingWindow.front());
            slidingWindow.pop_front();
            base++;
        }
        if (base != nextSeqNum) {
            //如果窗口中有未发送包，则重新启动定时器
            pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);
        }
    }
}

void GBNSender::timeoutHandler(int seqNum) {
    pns->stopTimer(SENDER, seqNum);
    for (const Packet& pkt : this->slidingWindow) {
        pUtils->printPacket("S-! TimeOut, resend pkt: ", pkt);
        pns->sendToNetworkLayer(RECEIVER, pkt);
    }
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
    /*if(base!=nextSeqNum){
        pns->startTimer(SENDER,Configuration::TIME_OUT,base);
    }*/
}