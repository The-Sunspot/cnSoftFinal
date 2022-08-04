//
// Created by 光球层上的黑子 on 2022/8/4.
//
#include <iostream>
#include "../head/FinalCompetitionData.h"

//插入一个新数据，如果有会合并
void FinalCompetitionData::insertPartData(PartSaleData data) {
    std::lock_guard lg(localLock);
    if(!this->partidHash.count(data.partid)){
        partidHash[data.partid]=this->partData.size();
        partData.emplace_back(data);
    }
    else{
        int id=partidHash[data.partid];
        partData[id].merge(data);
    }
}

//对一个订单，统计其数据
void FinalCompetitionData::solveAnOrder(ZipOrder &zipOrder) {
    this->insertPartData({zipOrder.lPartkey,zipOrder.lQuantity,1});
}

void FinalCompetitionData::insertReceivePartData(PartSaleData data) {
    std::lock_guard lg(receiveLock);
    if(!this->receiveidHash.count(data.partid)){
        receiveidHash[data.partid]=this->receivePartData.size();
        receivePartData.emplace_back(data);
    }
    else{
        int id=receiveidHash[data.partid];
        receivePartData[id].merge(data);
    }
}

void FinalCompetitionData::mergeData() {
    for(auto &i:receivePartData){
        insertPartData(i);
    }
}

void FinalCompetitionData::addUnderSales(long double d) {
    std::lock_guard lg(SalesLock);
    underSales+=d;
}

void FinalCompetitionData::setUnderSales(long double d) {
    std::lock_guard lg(SalesLock);
    underSales=d;
}
