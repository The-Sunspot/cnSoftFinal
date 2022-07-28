//
// Created by 光球层上的黑子 on 2022/7/28.
//

#include <iomanip>
#include "../head/QueryResponser.h"
#include "iostream"

void QueryResponser::selectAll() {
    this->totalData.outputUnderSale();
}

QueryResponser::QueryResponser(Calculator &c, TotalData &d):calculator(c),totalData(d){}

void QueryResponser::selectBrand(pair<int, int> &brand) {
    this->totalData.outputUnderSale(brand);
}

void QueryResponser::printSaleInfo() {
    std::cout << "-------------------------------------------------\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "totalSales:" << totalData.totalSales << "\n";
    std::cout << "totalPartCount:" << totalData.partCount << "\n";
    std::cout << "avg:" << totalData.getAvg() << "\n";
    std::cout << "-------------------------------------------------\n";
    totalData.outputUnderSale();
    std::cout << "-------------------------------------------------" << std::endl;
}

void QueryResponser::selectContainer(std::string container) {
    totalData.outputUnderSaleByContainer(container);
}

void QueryResponser::selectBrandAndContainer(pair<int, int> &brand, string &container) {
    totalData.outputUnderSaleByBrandAndContainer(brand,container);
}
