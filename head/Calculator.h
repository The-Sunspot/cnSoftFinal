//
// Created by 光球层上的黑子 on 2022/6/24.
//

#ifndef CNSOFTBEI_CALCULATOR_H
#define CNSOFTBEI_CALCULATOR_H

#include "TotalData.h"
#include "dataReader.h"
#include "client.h"
#include "Utils.h"
#include "FinalCompetitionData.h"
#include <mutex>
#ifdef _WIN64
#define TIME_TRANS 1000
#else
#define TIME_TRANS 1000000
#endif
//基本计算类
//目前使用改进的AdvancedCalculator类，其为Calculator的子类
class Calculator
{
private:
    mutex mtx;

public:
    // 用于finish的同步的flag
    int index, totalProgramCount;
    bool is_finished, is_notify;
    bool select_flag = false;
    int select_idx;
    //统计信息对象
    TotalData &totalData;
    //数据读入对象
    dataReader &reader;
    //消息发送对象
    client *sendMessager;
    //决赛统计数据
    FinalCompetitionData data;
    //读取数据，并记录时间
    void loadDatas();
    //修改flag量
    void setFinishAndNotify(bool f, bool t);
    void setNotify(bool t);
    //计算主控函数
    void doCalculate(condition_variable &cv, std::unique_lock<std::mutex> &lck);
    //订单计算子函数
    void solveOrder(ZipOrder &order);
    //尝试插入一个不畅销
    bool tryInsertUndersalePart(PartData part);
    //不畅销商品的转发
    void transferUndersalePartData(message m);
    //构造函数
    Calculator(TotalData &totalData, dataReader &reader, client *sendMessager = nullptr);
    //传递结束信息
    void sendFinish(const char *cnt = " 0000");
    //检查是否全部结束
    bool checkFinish(const char *cnt) const;
    //发送统计信息
    void sendTotalData(int port, int partCount, long long totalSale) const;
    //传递统计信息
    void transferTotalData(const message &m) const;
    //发送零件信息
    void sendPart(ZipPart &part);
    //进程间同步
    void doAsync(condition_variable &cv, unique_lock<mutex> &lck);
};

#endif // CNSOFTBEI_CALCULATOR_H
