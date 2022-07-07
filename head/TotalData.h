//
// Created by 光球层上的黑子 on 2022/6/23.
//

#ifndef CNSOFTBEI_TOTALDATA_H
#define CNSOFTBEI_TOTALDATA_H

#include <iostream>
#include "BrandData.h"
#include <mutex>

//答案统计类
//内含零件数、销售量等几个总统计数据
//以及一个5*5的BrandData数组，每个代表一个品牌的数据信息
class TotalData
{
private:

    //总销售量锁，总零件数锁
    std::mutex salesLock,countLock;
    //本机销售量锁，本机零件锁
    std::mutex localSalesLock,localCountLock;
public:

    // 5*5数组，datas[i][j]存放brand i+1 j+1数据
    std::vector<std::vector<BrandData *>> datas;
    //总销售数量,本机销售数量
    long long totalSales=0,localSales=0;
    //总零件数量，本机零件数量
    int partCount=0,localPartCount=0;

    //输出某个品牌的不畅销产品
    void outputUnderSale(std::pair<int,int>&brand);
    //返回平均值
    double getAvg() const;
    //计算不畅销，并统计
    void calculateUnderSale();
    //输出所有UnderSale商品信息
    void outputUnderSale();
    //尝试插入一个不畅销产品，只有在不存在时才会插入
    //插入成功返回true，否则false
    bool tryInsertUnderSale(PartData part);
    //合并一个PartData
    void mergePartData(PartData &part);
    //带锁增加本地零件数目
    void addLocalPartCount(int d);
    //带锁设置本地零件数目
    void setLocalPartCount(int newCount);
    //带锁增加本地销量
    void addLocalTotalSales(long long d);
    //带锁修改本地销量
    void setLocalTotalSales(long long newSale);
    //带锁增加零件数目
    void addPartCount(int d);
    //带锁修改零件数目
    void setPartCount(int newCount);
    //带锁增加销售量
    void addTotalSales(long long d);
    //带锁修改销售量
    void setTotalSales(long long newSale);
    TotalData();
    ~TotalData();
};

#endif // CNSOFTBEI_TOTALDATA_H
