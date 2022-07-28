//
// Created by 光球层上的黑子 on 2022/7/28.
//

#ifndef CNSOFTBEI_QUERYRESPONSER_H
#define CNSOFTBEI_QUERYRESPONSER_H
#include "Calculator.h"
#include "dataReader.h"

class QueryResponser {
private:
    Calculator &calculator;
    TotalData &totalData;
public:
    QueryResponser(Calculator &c,TotalData &d);
    void printSaleInfo();
    void selectAll();
    void selectBrand(std::pair<int,int>&brand);
};


#endif //CNSOFTBEI_QUERYRESPONSER_H
