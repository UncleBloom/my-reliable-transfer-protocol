//
// Created by 王一舟 on 2021/10/28.
//

#include <TCPSender.h>

bool inline TCPSender::inWindow(int rcvSeqNum) const {
    return rcvSeqNum >= base && rcvSeqNum <= (base + windowSize - 1);
}

TCPSender::TCPSender() : base(0), windowSize(WINDOW_SIZE), nextSeqNum(0), lastAckTimes(0) {}

/*
 * 若发送窗口已满则返回true
 */
bool inline TCPSender::getWaitingState() {
    return nextSeqNum > (base + windowSize - 1);
}

/**
 * @description 发送应用层的Message，由NetworkService调用
 * @return 如果成功地将Message发送到网络层，返回true;如果发送窗口已满而拒绝发送Message，则返回false
*/
bool TCPSender::send(const Message &message) {
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
void TCPSender::receive(const Packet &ackPkt) {
    //检查校验和
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    int ackNum = ackPkt.acknum;
    if (checkSum == ackPkt.checksum && inWindow(ackNum)) {
        //如果校验和正确，并且确认号在窗口内
        pUtils->printPacket("S-- RcvAck: ", ackPkt);
        pns->stopTimer(SENDER, this->base);
        if (ackNum > base) {
            //确认号大于基序号则触发累计确认
            while (ackNum >= base) {
                pUtils->printPacket(("$ Slid window from " + to_string(this->base) + ": ").data(), this->slidingWindow.front());
                slidingWindow.pop_front();
                base++;
            }
            lastAckTimes = 1;
            if (base != nextSeqNum) {
                //如果窗口中有未发送包，则重新启动定时器
                pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);
            }
        } else if (ackNum == base) {
            //确认号等于基序号则记录确认次数
            lastAckTimes++;
            if (lastAckTimes >= 3) {
                //触发快速重传
                pns->stopTimer(SENDER, this->base);
                Packet packet = *slidingWindow.begin();
                pUtils->printPacket("$ S-! quickResend pkt: ", packet);
                pns->sendToNetworkLayer(RECEIVER, packet);
                pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);
            }
        }
    }
}

void TCPSender::timeoutHandler(int seqNum) {
    pns->stopTimer(SENDER, seqNum);
    //仅重传未被确认的第一个包
    Packet packet = *slidingWindow.begin();
    pUtils->printPacket("S-! TimeOut, resend pkt: ", packet);
    pns->sendToNetworkLayer(RECEIVER, packet);
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
}