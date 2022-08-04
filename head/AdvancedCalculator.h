//
// Created by 光球层上的黑子 on 2022/7/5.
//

#ifndef CNSOFTBEI_ADVANCEDCALCULATOR_H
#define CNSOFTBEI_ADVANCEDCALCULATOR_H
#include "Calculator.h"

//改进版计算类
//其核心算法分为三步
class AdvancedCalculator : public Calculator
{

public:
    AdvancedCalculator(TotalData &totalData, dataReader &reader, client *sendMessager = nullptr) : Calculator(totalData, reader, sendMessager) {}
    //改进版计算
    void doCalculate(condition_variable &cv, std::unique_lock<std::mutex> &lck);

private:
    //核心计算第一步，进程通信自己的零件信息
    void partTabelCommunicate(condition_variable &cv, unique_lock<std::mutex> &lck);
    //核心计算第二步，进程在本地进行计算统计
    void localCalculate(condition_variable &cv, unique_lock<std::mutex> &lck);
    //核心计算第三步，进程之间通信统计后的零件信息
    void partDataCommunicate(condition_variable &cv, unique_lock<std::mutex> &lck);
    //改进版订单处理,是第二步的辅助函数
    void solveOrder(ZipOrder &order);
    //统计出总销售量、销售额，输出部分的辅助函数
    void getTotalDatas();
};

#endif // CNSOFTBEI_ADVANCEDCALCULATOR_H
