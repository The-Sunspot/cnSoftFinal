//
// Created by 光球层上的黑子 on 2022/7/28.
//

#ifndef CNSOFTBEI_QUERYRESPONSER_H
#define CNSOFTBEI_QUERYRESPONSER_H

#include <random>
#include "Calculator.h"
#include "dataReader.h"

class QueryResponser {
private:
    Calculator &calculator;
    TotalData &totalData;
    //随机container
    vector<string>containers={"WRAP JAR","MED DRUM","SM BOX","JUMBO BOX","LG BAG","SM CAN",
                "MED CAN","WRAP DRUM","LG PACK","SM JAR","WRAP PACK","MED BAG","LG DRUM",
                "JUMBO JAR","JUMBO PKG","MED PKG","MED CASE","LG BOX","WRAP CAN","JUMBO BAG",
                "WRAP BOX","SM CASE","LG CASE","LG PKG","LG CAN","MED JAR","LG JAR","JUMBO PACK",
                "SM DRUM","SM PACK","SM BAG","MED BOX","MED PACK","SM PKG","JUMBO CAN","WRAP BAG",
                "JUMBO CASE","WRAP PKG","JUMBO DRUM","WRAP CASE"
    };
    //随机数发生器
    std::default_random_engine randomEngine;
public:
    QueryResponser(Calculator &c,TotalData &d);
    void printSaleInfo();
    void selectAll();
    void selectBrand(std::pair<int,int>&brand);
    void selectContainer(std::string container);
    void selectBrandAndContainer(std::pair<int,int>&brand,std::string&container);
    void randomSelect();
    void dealInteract(const string &s);
};


#endif //CNSOFTBEI_QUERYRESPONSER_H
