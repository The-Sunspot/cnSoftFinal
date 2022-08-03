//
// Created by 光球层上的黑子 on 2022/8/4.
//

#ifndef CNSOFTBEI_PARTSALEDATA_H
#define CNSOFTBEI_PARTSALEDATA_H


#include <mutex>
class PartSaleData {
public:
    PartSaleData(int id,int sale,int count):partid(id),totalSale(sale),orderCount(count),lock(){}
    PartSaleData(const PartSaleData  &data):partid(data.partid),totalSale(data.totalSale),orderCount(data.orderCount),lock(){}
    int partid;// 主键
    int totalSale;// 总得销售量
    int orderCount;// 总得订单数量
    //long double avgSale;
    std::mutex lock;
    //带锁合并两个数据，第一步调用
    void merge(const PartSaleData& data);
    //获取平均销售量，应当在第一步同步后调用才能为合理值.
    [[nodiscard]] long double getAvg() const;
};


#endif //CNSOFTBEI_PARTSALEDATA_H
