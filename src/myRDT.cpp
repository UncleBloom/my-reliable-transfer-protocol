//
// Created by 王一舟 on 2021/10/22.
//

// StopWait.cpp : 定义控制台应用程序的入口点。
#include "../include/GBNReceiver.h"
#include "../include/GBNSender.h"
#include "../include/Global.h"
#include "../include/StopWaitRdtReceiver.h"
#include "../include/StopWaitRdtSender.h"
#include <SRReceiver.h>
#include <SRSender.h>
#include <TCPReceiver.h>
#include <TCPSender.h>

int main(int argc, char *argv[]) {
    int type;
    cout << "choose protocol type:\n1 for GBN\n2 for SR\n3 for TCP\n0 for StopWait\n";
    cin >> type;

    RdtSender *ps = nullptr;
    RdtReceiver *pr = nullptr;
    switch (type) {
        case 1:
            ps = new GBNSender;
            pr = new GBNReceiver;
            break;
        case 2:
            ps = new SRSender;
            pr = new SRReceiver;
            break;
        case 3:
            ps = new TCPSender;
            pr = new TCPReceiver;
            break;
        case 0:
            ps = new StopWaitRdtSender();
            pr = new StopWaitRdtReceiver();
            break;
        default:
            cout << "wrong choice" << endl;
            return 0;
    }
    //pns->setRunMode(0);//VERBOS模式
    pns->setRunMode(1);//安静模式
    pns->init();
    pns->setRtdSender(ps);
    pns->setRtdReceiver(pr);
    pns->setInputFile("../input.txt");
    pns->setOutputFile("../output.txt");

    pns->start();

    delete ps;
    delete pr;
    delete pUtils;//指向唯一的工具类实例，只在main函数结束前delete
    delete pns;   //指向唯一的模拟网络环境类实例，只在main函数结束前delete

    return 0;
}
