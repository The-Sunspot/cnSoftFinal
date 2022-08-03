//
// Created by 光球层上的黑子 on 2022/8/4.
//

#ifndef CNSOFTBEI_FINALCOMPETITIONDATA_H
#define CNSOFTBEI_FINALCOMPETITIONDATA_H

#include <vector>
#include <unordered_map>
#include "PartSaleData.h"
#include "ZipOrder.h"
class FinalCompetitionData {
public:

    FinalCompetitionData():underSales(0){};
    //本机数据
    std::unordered_map<int,int>partidHash;
    std::vector<PartSaleData>partData;
    std::mutex localLock;
    //收到的，不能直接插入，防止并发问题
    std::unordered_map<int,int>receiveidHash;
    std::vector<PartSaleData>receivePartData;
    std::mutex receiveLock;

    long double underSales;//最终答案除以7之前的值
    std::mutex SalesLock;
    void addUnderSales(long double d);
    void setUnderSales(long double d);

    //插入一个PartSaleData
    void insertPartData(PartSaleData data);
    void solveAnOrder(ZipOrder&zipOrder);
    //服务器插入
    void insertReceivePartData(PartSaleData data);
    //本机与接收的合并
    void mergeData();
};


#endif //CNSOFTBEI_FINALCOMPETITIONDATA_H
