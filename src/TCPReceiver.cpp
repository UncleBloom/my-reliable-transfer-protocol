//
// Created by 王一舟 on 2021/10/28.
//

#include <TCPReceiver.h>
bool inline TCPReceiver::inWindow(int rcvSeqNum) const {
    return rcvSeqNum >= base && rcvSeqNum <= (base + windowSize - 1);
}

TCPReceiver::TCPReceiver() : base(0), windowSize(WINDOW_SIZE) {}

void TCPReceiver::receive(const Packet &packet) {
    //计算校验和
    int checkSum = pUtils->calculateCheckSum(packet);
    if (checkSum != packet.checksum) {
        //如果校验和不正确则直接丢弃报文
        return;
    }

    //读取序列号并生成确认报文
    int seqNum = packet.seqnum;
    Packet ackPkt;
    ackPkt.acknum = seqNum;
    ackPkt.seqnum = -1;//忽略该字段
    for (char &i : ackPkt.payload) {
        i = '.';
    }

    //如果收到报文不在滑动窗口内
    if (! inWindow(seqNum)) {
        //为序号在[base-windowSize, base-1]内的分组重新发送ACK
        ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
        pUtils->printPacket("R-! Pkt out of window. ResendAck: ", packet);
        pns->sendToNetworkLayer(SENDER, ackPkt);
    } else if (seqNum == this->base) {
        //如果序号等于基序号则将分组交付给上层
        pUtils->printPacket("R-- receive: ", packet);//接收方正确收到发送方的报文
        this->slidingWindow[seqNum] = packet;
        while (this->slidingWindow.count(base)) {
            Message message;
            memcpy(message.data, slidingWindow[base].payload, sizeof(packet.payload));
            pUtils->printPacket(("$R Slid window from " + to_string(this->base) + ": ").data(), this->slidingWindow[base]);
            this->slidingWindow.erase(base);
            pns->delivertoAppLayer(RECEIVER, message);
            this->base++;
        }
        ackPkt.acknum = this->base - 1;
        ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
        pns->sendToNetworkLayer(SENDER, ackPkt);
    } else {
        //序号在窗口内但不为基序号，则再次发送对基序号前一个包的Ack
        ackPkt.acknum = this->base - 1;
        ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
        pUtils->printPacket("R-- receive not expecting: ", packet);
        pUtils->printPacket("R-- Resend Ack for expecting: ", ackPkt);
        pns->sendToNetworkLayer(SENDER, ackPkt);
    }
}
