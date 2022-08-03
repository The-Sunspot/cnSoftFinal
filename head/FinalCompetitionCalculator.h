//
// Created by 光球层上的黑子 on 2022/8/4.
//

#ifndef CNSOFTBEI_FINALCOMPETITIONCALCULATOR_H
#define CNSOFTBEI_FINALCOMPETITIONCALCULATOR_H
#include "Calculator.h"

#include <utility>
#include "FinalCompetitionData.h"
class FinalCompetitionCalculator : public Calculator{
public:
    //查询参数
    string targetContainer;
    pair<int,int> targetBrand;



    //订单
    vector<ZipOrder> orders;

    void doCalculate(condition_variable &cv, std::unique_lock<std::mutex> &lck);
    FinalCompetitionCalculator(TotalData &totalData, dataReader &reader, client *sendMessager = nullptr) : Calculator(totalData, reader, sendMessager) {
        targetContainer=Utils::getRandomContainer();
        targetBrand=Utils::getRandomBrand();
    }
    FinalCompetitionCalculator(pair<int,int>brand,string container,TotalData &totalData, dataReader &reader, client *sendMessager = nullptr) : Calculator(totalData, reader, sendMessager) {
        targetContainer=std::move(container);
        targetBrand=brand;
    }

private:
    //第一步，预处理
    void preprocess(condition_variable &cv, unique_lock<std::mutex> &lck);
    //第二步，通信partSaleData
    void communicatePartSale(condition_variable &cv, unique_lock<std::mutex> &lck);
    //第三步，本地计算答案
    void getLocalUnderSale(condition_variable &cv, unique_lock<std::mutex> &lck);
    //第四步，发送，输出
    void mergeQueryAns(condition_variable &cv, unique_lock<std::mutex> &lck);
    void localCalculate();
};


#endif //CNSOFTBEI_FINALCOMPETITIONCALCULATOR_H
