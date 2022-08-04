//
// Created by 光球层上的黑子 on 2022/6/15.
//

#include "../head/BrandData.h"
//根据订单更新信息
void BrandData::updateData(ZipOrder &order,const std::string &container) {
    using namespace std;
    lock_guard lg(this->m_mtx);
    //第一次出现这个零件，就先加入
    if (!this->partIdHash.count(order.lPartkey))
    {
        this->partIdHash[order.lPartkey] = this->parts.size();
        this->parts.emplace_back(PartData(order.lPartkey, {this->brandCode.first, this->brandCode.second},container));
    }

    //找到这个零件的统计数据
    auto &targetPartData = this->parts[this->partIdHash[order.lPartkey]];
    targetPartData.saleCount += order.lQuantity;
    targetPartData.totalSales += order.lQuantity * order.lExtendedprice;
}
//根据partData更新信息
void BrandData::mergePartData(PartData &part){
    //std::lock_guard lg(underSaleMutex);
    if(!partIdHash.count(part.id)){
        partIdHash.insert({part.id,parts.size()});
        parts.emplace_back(part);
        return;
    }
    int partIndex=partIdHash[part.id];
    auto &target=parts[partIndex];
    target.totalSales+=part.totalSales;
    target.saleCount+=part.saleCount;
}
//插入一个不畅销产品
void BrandData::insertUnderSale(PartData &part){
    std::lock_guard lg(underSaleMutex);
    underSaleParts.emplace_back(part);
    underSalePartsHash.insert(part.id);
}
//是否存在这个商品，存在返回true，否则false
bool BrandData::existThisPart(PartData &part){
    //根据hash表查询
    return underSalePartsHash.count(part.id)!=0;
}
//输出这个品牌不畅销信息
void BrandData::outputUnderSale(){
    //std::cout<<"id Sales SaleCount Brand Container"<<std::endl;
    for(auto &i:underSaleParts){
        std::cout<<i.id<<" "<<i.totalSales<<" "<<i.saleCount<<" "<<"Brand#"<<i.brandCode.first*10+i.brandCode.second<<' '<<i.container<<std::endl;
    }
}

void BrandData::outputUnderSaleByContainer(const std::string &container) {
    //std::cout<<"id Sales SaleCount Brand Container"<<std::endl;
    for(auto &i:underSaleParts){
        if(i.container==container)
            std::cout<<i.id<<" "<<i.totalSales<<" "<<i.saleCount<<" "<<"Brand#"<<i.brandCode.first*10+i.brandCode.second<<' '<<i.container<<std::endl;
    }
}
