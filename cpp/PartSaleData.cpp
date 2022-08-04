//
// Created by 光球层上的黑子 on 2022/8/4.
//

#include "../head/PartSaleData.h"

//带锁合并两个数据，第一步调用
void PartSaleData::merge(const PartSaleData &data) {
    std::lock_guard lg(lock);
    this->orderCount+=data.orderCount;
    this->totalSale+=data.totalSale;
}
//获取平均销售量，应当在第一步同步后调用才能为合理值.
long double PartSaleData::getAvg() const {
    return ((long double)this->totalSale)/((long double)this->orderCount);
}
