//
// Created by 光球层上的黑子 on 2022/6/15.
//

#ifndef CNSOFTBEI_BRANDDATA_H
#define CNSOFTBEI_BRANDDATA_H

#include <vector>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include "PartData.h"
#include "ZipOrder.h"
#include "ZipPart.h"
#include <set>

class BrandData
{
private:
    mutable std::mutex m_mtx;
    std::mutex underSaleMutex;
    //不畅销零件
    std::vector<PartData> underSaleParts;
    //不畅销零件hash
    std::unordered_set<int> underSalePartsHash;
public:
    //这个品牌的编号
    std::pair<int, int> brandCode;
    //这个品牌的零件
    std::vector<PartData> parts;
    //这个品牌零件的hash
    std::unordered_map<int, int> partIdHash;
    //所有零件销售量
    int brandSaleCount=0;
    //所有零件销售额
    double brandSales=0;
    //根据订单更新信息
    void updateData(ZipOrder &order);
    //根据partData更新信息
    void mergePartData(PartData &part);
    //插入一个不畅销产品
    void insertUnderSale(PartData &part);
    //是否存在这个商品，存在返回true，否则false
    bool existThisPart(PartData &part);
    //输出这个品牌不畅销信息
    void outputUnderSale();
};

#endif // CNSOFTBEI_BRANDDATA_H
