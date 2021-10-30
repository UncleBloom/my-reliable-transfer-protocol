//
// Created by 王一舟 on 2021/10/27.
//
#include "../include/SRReceiver.h"
#include "../include/Global.h"

bool inline SRReceiver::inWindow(int rcvSeqNum) const {
    return rcvSeqNum >= base && rcvSeqNum <= (base + windowSize - 1);
}

SRReceiver::SRReceiver() : base(0), windowSize(WINDOW_SIZE) {}

void SRReceiver::receive(const Packet &packet) {
    //计算校验和
    int checkSum = pUtils->calculateCheckSum(packet);

    //读取序列号并生成确认报文
    int seqNum = packet.seqnum;
    Packet ackPkt;
    ackPkt.acknum = seqNum;
    ackPkt.seqnum = -1;//忽略该字段
    for (char &i : ackPkt.payload) {
        i = '.';
    }
    ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
    //如果校验和正确，同时收到报文的序号等于接收方期待收到的报文序号一致
    if (checkSum == packet.checksum && inWindow(seqNum)) {
        pUtils->printPacket("R-- receive: ", packet);//接收方正确收到发送方的报文
        pns->sendToNetworkLayer(SENDER, ackPkt);
        this->slidingWindow[seqNum] = packet;

        //如果序号等于基序号则将分组交付给上层
        while (this->slidingWindow.count(base)) {
            Message message;
            memcpy(message.data, slidingWindow[base].payload, sizeof(packet.payload));
            pUtils->printPacket(("$R Slid window from " + to_string(this->base) + ": ").data(), this->slidingWindow[base]);
            this->slidingWindow.erase(base);
            pns->delivertoAppLayer(RECEIVER, message);
            this->base++;
        }
    } else if (seqNum >= base - windowSize && seqNum < base) {
        //为序号在[base-windowSize, base-1]内的分组重新发送ACK
        pUtils->printPacket("R-! ResendAck: ", packet);
        pns->sendToNetworkLayer(SENDER, ackPkt);
    }
}
