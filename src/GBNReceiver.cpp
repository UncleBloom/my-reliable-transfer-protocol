//
// Created by 王一舟 on 2021/10/23.
//

#include "../include/GBNReceiver.h"
#include "../include/Global.h"

GBNReceiver::GBNReceiver() : expectSeqNum(0) {
    lastAckPkt.acknum = -1;//初始状态，上次发送的确认包的确认序号为-1，第一个接受的数据包出错时该确认报文的确认号为-1
    lastAckPkt.checksum = 0;
    lastAckPkt.seqnum = -1;//忽略该字段
    for (char &i : lastAckPkt.payload) {
        i = '.';
    }
    lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}

void GBNReceiver::receive(const Packet &packet) {
    //计算校验和
    int checkSum = pUtils->calculateCheckSum(packet);

    //如果校验和正确，同时收到报文的序号等于接收方期待收到的报文序号一致
    if (checkSum == packet.checksum && this->expectSeqNum == packet.seqnum) {
        pUtils->printPacket("R-- receive: ", packet);//接收方正确收到发送方的报文

        //取出Message，向上递交给应用层
        Message msg;
        memcpy(msg.data, packet.payload, sizeof(packet.payload));
        pns->delivertoAppLayer(RECEIVER, msg);

        lastAckPkt.acknum = packet.seqnum;//确认序号等于收到的报文序号
        lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
        pUtils->printPacket("R-- sendACK: ", lastAckPkt);//接收方发送确认报文
        pns->sendToNetworkLayer(SENDER, lastAckPkt);     //调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方

        this->expectSeqNum++;//接收序号+1
    } else {
        if (checkSum != packet.checksum) {
            pUtils->printPacket("R-! CheckNumError: ", packet);//接收方没有正确收到发送方的报文,数据校验错误
        } else {
            pUtils->printPacket("R-! SeqNumError: ", packet);//接收方没有正确收到发送方的报文,报文序号不对
        }
        pUtils->printPacket("R-! resendACK: ", lastAckPkt);//接收方重新发送上次的确认报文
        pns->sendToNetworkLayer(SENDER, lastAckPkt);       //调用模拟网络环境的sendToNetworkLayer，通过网络层发送上次的确认报文
    }
}
