//
// Created by 光球层上的黑子 on 2022/6/23.
//

#include "../head/TotalData.h"

//申请内存
TotalData::TotalData() : totalSales(0)
{
    datas = std::vector<std::vector<BrandData *>>{5, std::vector<BrandData *>(5)};
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            datas[i][j] = new BrandData();
            datas[i][j]->brandCode = {i + 1, j + 1};
        }
    }
}
//释放内存
TotalData::~TotalData()
{
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            delete datas[i][j];
        }
    }
}

//带锁修改销售量
void TotalData::setTotalSales(long long int newSale)
{
    std::lock_guard lg(salesLock);
    totalSales = newSale;
}

//带锁增加销售量
void TotalData::addTotalSales(long long int d)
{
    std::lock_guard lg(salesLock);
    totalSales += d;
}

//带锁修改零件数目
void TotalData::setPartCount(int newCount)
{
    std::lock_guard lg(countLock);
    partCount = newCount;
}

//带锁增加零件数目
void TotalData::addPartCount(int d)
{
    std::lock_guard lg(countLock);
    partCount += d;
}

//带锁修改本地销量
void TotalData::setLocalTotalSales(long long int newSale)
{
    std::lock_guard lg(localSalesLock);
    localSales = newSale;
}

//带锁增加本地销量
void TotalData::addLocalTotalSales(long long int d)
{
    std::lock_guard lg(localSalesLock);
    localSales += d;
}

//带锁设置本地零件数目
void TotalData::setLocalPartCount(int newCount)
{
    std::lock_guard lg(localCountLock);
    localPartCount = newCount;
}

//带锁增加本地零件数目
void TotalData::addLocalPartCount(int d)
{
    std::lock_guard lg(localCountLock);
    localPartCount += d;
}

//合并一个PartData
void TotalData::mergePartData(PartData &part)
{
    auto &brand = part.brandCode;
    this->datas[brand.first - 1][brand.second - 1]->mergePartData(part);
}

//尝试插入一个不畅销产品，只有在不存在时才会插入
//插入成功返回true，否则false
bool TotalData::tryInsertUnderSale(PartData part)
{
    auto &d = datas[part.brandCode.first - 1][part.brandCode.second - 1];
    if (d->existThisPart(part))
        return false;
    d->insertUnderSale(part);
    return true;
}

//输出所有UnderSale商品信息
void TotalData::outputUnderSale()
{
    //遍历所有品牌
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            //调用品牌自己的输出函数
            auto &brandData = datas[i][j];
            std::cout << "Brand#" << i + 1 << j + 1 << ": Sales: " << brandData->brandSales << " SaleCount: " << brandData->brandSaleCount << std::endl;
            brandData->outputUnderSale();
            std::cout << "----------------------\n";
        }
    }
    std::cout << std::endl;
}

//计算不畅销，并统计
void TotalData::calculateUnderSale()
{
    double avg = getAvg();
    //遍历
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            auto &brandData = datas[i][j];
            //遍历品牌所有零件
            for (auto &part : brandData->parts)
            {
                //根据题目要求，统计小于平均销量30%的商品
                if (part.saleCount < avg * 0.3)
                {
                    //为品牌统计
                    brandData->brandSaleCount += part.saleCount;
                    brandData->brandSales += part.totalSales;
                    //记录不畅销
                    brandData->insertUnderSale(part);
                }
            }
        }
    }
}

//返回平均值
double TotalData::getAvg() const
{
    return (totalSales + 0.) / partCount;
}

//输出某个品牌的不畅销产品
void TotalData::outputUnderSale(std::pair<int, int> &brand)
{
    auto &brandData = datas[brand.first - 1][brand.second - 1];
    std::cout << "Brand#" << brand.first << brand.second << ": Sales: " << brandData->brandSales << " SaleCount: " << brandData->brandSaleCount << std::endl;
    brandData->outputUnderSale();
    std::cout << "----------------------" << std::endl;
}

void TotalData::outputUnderSaleByContainer(std::string &container) {
    //遍历所有品牌
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            //调用品牌自己的输出函数
            auto &brandData = datas[i][j];
            //std::cout << "Brand#" << i + 1 << j + 1 << ": Sales: " << brandData->brandSales << " SaleCount: " << brandData->brandSaleCount << std::endl;
            brandData->outputUnderSaleByContainer(container);
        }
    }
    std::cout << "----------------------\n";
    std::cout << std::endl;
}

void TotalData::outputUnderSaleByBrandAndContainer(std::pair<int, int> &brand, std::string &container) {
    auto &brandData = datas[brand.first - 1][brand.second - 1];
    //std::cout << "Brand#" << brand.first << brand.second << ": Sales: " << brandData->brandSales << " SaleCount: " << brandData->brandSaleCount << std::endl;
    brandData->outputUnderSaleByContainer(container);
    std::cout << "----------------------" << std::endl;
}
