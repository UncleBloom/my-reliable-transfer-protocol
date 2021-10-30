//
// Created by 王一舟 on 2021/10/27.
//
#include <Global.h>
#include <SRSender.h>

bool inline SRSender::inWindow(int rcvSeqNum) const {
    return rcvSeqNum >= base && rcvSeqNum <= (base + windowSize - 1);
}
SRSender::SRSender() : base(0), windowSize(WINDOW_SIZE), nextSeqNum(0) {}

/*
 * 若发送窗口已满则返回true
 */
bool inline SRSender::getWaitingState() {
    return nextSeqNum > (base + windowSize - 1);
}

/**
 * @description 发送应用层的Message，由NetworkService调用
 * @return 如果成功地将Message发送到网络层，返回true;如果发送窗口已满而拒绝发送Message，则返回false
*/
bool SRSender::send(const Message &message) {
    //发送窗口已满
    if (getWaitingState()) { return false; }

    Packet packet;
    packet.acknum = -1;//忽略该字段
    packet.seqnum = this->nextSeqNum;
    memcpy(packet.payload, message.data, sizeof(message.data));
    packet.checksum = pUtils->calculateCheckSum(packet);

    //将包放入滑动窗口，生成定时器，并发送报文
    slidingWindow.emplace_back(sent, packet);
    pUtils->printPacket("S-- sendPkt: ", packet);
    pns->startTimer(SENDER, Configuration::TIME_OUT, nextSeqNum);
    pns->sendToNetworkLayer(RECEIVER, packet);

    nextSeqNum++;
    return true;
}

/**
 * @description 接受确认Ack，将被NetworkService调用
 * @param ackPkt 接受的ACK包
 */
void SRSender::receive(const Packet &ackPkt) {
    //检查校验和
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    int ackNum = ackPkt.acknum;
    if (checkSum == ackPkt.checksum && inWindow(ackNum)) {
        //如果校验和正确，并且确认号在窗口内
        pUtils->printPacket("S-- RcvAck: ", ackPkt);
        pns->stopTimer(SENDER, ackNum);
        //将对应包在滑动窗口内的状态设为已确认
        auto it = this->slidingWindow.begin();
        for (int i = 0; i < (ackNum - this->base); i++) {
            it++;
        }
        it->first = confirmed;
    }
    if (ackNum == this->base) {
        //如果确认号等于基序号，就向后移动滑动窗口
        while (this->slidingWindow.begin()->first == confirmed) {
            //删除缓存区中已被确认的包，并更新滑动窗口
            pUtils->printPacket(("$S Slid window from " + to_string(this->base) + ": ").data(), this->slidingWindow.front().second);
            this->slidingWindow.pop_front();
            this->base++;
        }
    }
}

void SRSender::timeoutHandler(int seqNum) {
    pns->stopTimer(SENDER, seqNum);
    auto it = this->slidingWindow.begin();
    for (int i = 0; i < (seqNum - this->base); i++) {
        it++;
    }
    pUtils->printPacket("S-! TimeOut, resend pkt: ", it->second);
    pns->sendToNetworkLayer(RECEIVER, it->second);
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
}
