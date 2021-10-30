//
// Created by 王一舟 on 2021/10/22.
//

#ifndef STOPWAIT_GLOBAL_H
#define STOPWAIT_GLOBAL_H

#include "NetworkService.h"
#include "Tool.h"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
using namespace std;

extern Tool *pUtils;       //指向唯一的工具类实例，只在main函数结束前delete
extern NetworkService *pns;//指向唯一的模拟网络环境类实例，只在main函数结束前delete

#endif//STOPWAIT_GLOBAL_H
