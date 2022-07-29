//
// Created by 光球层上的黑子 on 2022/7/28.
//

#include <iomanip>
#include "../head/QueryResponser.h"
#include "iostream"

void QueryResponser::selectAll() {
    this->totalData.outputUnderSale();
}

QueryResponser::QueryResponser(Calculator &c, TotalData &d):calculator(c),totalData(d), randomEngine(time(nullptr)){}

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
//    totalData.outputUnderSale();
//    std::cout << "-------------------------------------------------" << std::endl;
}

void QueryResponser::selectContainer(std::string container) {
    totalData.outputUnderSaleByContainer(container);
}

void QueryResponser::selectBrandAndContainer(pair<int, int> &brand, string &container) {
    totalData.outputUnderSaleByBrandAndContainer(brand,container);
}

void QueryResponser::randomSelect() {
    uniform_int_distribution<int> brandGen(1,5);
    uniform_int_distribution<int> containerGen(0,containers.size()-1);
    pair<int,int> brand={brandGen(randomEngine),brandGen(randomEngine)};
    string container=this->containers[containerGen(randomEngine)];
    cout<<"random para: "<<"Brand#"<<brand.first<<brand.second<<"   container: "<<container<<endl;
    this->selectBrand(brand);
    cout<<"----------------------\n";
    this->selectContainer(container);
    cout<<"----------------------\n";
    this->selectBrandAndContainer(brand,container);
    cout<<"----------------------\n";
}

void QueryResponser::dealInteract(const string &s) {
    auto input=Utils::judgeInputExtended(s);
    if(get<0>(input)=="wrong"){
        cout
                << R"(input 'random' do random query, 'select b/brand {a}{b}' to query brand {a}{b},)"<<endl
                << R"('select c/container {con}' to query container {con},')"<<endl
                << R"('select b/brand {a}{b} c/container {con}' or 'select c/container {con} b/brand {a}{b}' to query brand{a}{b} and container {con}.)"
                << endl;
        cout << "input:";
    }
    else if(get<0>(input)=="random"){
        this->randomSelect();
        cout << "input:";
    }
    else if(get<0>(input)=="info"){
        this->printSaleInfo();
        cout << "input:";
    }
    else if(get<0>(input)=="all"){
        this->selectAll();
        cout << "input:";
    }
    else if(get<0>(input)=="brand"){
        auto brand=get<1>(input);
        if (brand == make_pair(-1, -1))
            cout
                    << R"(input 'random' do random query, 'select b/brand {a}{b}' to query brand {a}{b},)"<<endl
                    << R"('select c/container {con}' to query container {con},')"<<endl
                    << R"('select b/brand {a}{b} c/container {con}' or 'select c/container {con} b/brand {a}{b}' to query brand{a}{b} and container {con}.)"
                    << endl;
        else if (brand == make_pair(INT_MAX, INT_MAX))
            cout << "invalid brand code! (must belong to 1~5)" << endl;
        else
            this->selectBrand(brand);

        cout << "input:";
    }
    else if(get<0>(input)=="container"){
        this->selectContainer(get<2>(input));
        cout << "input:";
    }
    else if(get<0>(input)=="both"){
        auto brand=get<1>(input);
        if (brand == make_pair(-1, -1))
            cout
                    << R"(input 'random' do random query, 'select b/brand {a}{b}' to query brand {a}{b},)"<<endl
                    << R"('select c/container {con}' to query container {con},')"<<endl
                    << R"('select b/brand {a}{b} c/container {con}' or 'select c/container {con} b/brand {a}{b}' to query brand{a}{b} and container {con}.)"
                    << endl;
        else if (brand == make_pair(INT_MAX, INT_MAX))
            cout << "invalid brand code! (must belong to 1~5)" << endl;
        else{
            this->selectBrandAndContainer(brand,get<2>(input));
        }
        cout << "input:";
    }
}
