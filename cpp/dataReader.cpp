//
// Created by 光球层上的黑子 on 2022/6/15.
//

#include "../head/dataReader.h"
#include "../head/Utils.h"
#include <fstream>
#include <iostream>
#include <ctime>
#ifdef _WIN64
#define TIME_TRANS 1000
#else
#define TIME_TRANS 1000000
#endif

using namespace std;

//初始化
dataReader::dataReader(int index, int n, bool isTesting) : totalProgramCount(n), index(index)
{
    if (isTesting)
    {
        partFileNames[0] = R"(../data/testGoods)";
        orderFileName[0] = R"(../data/testSales.txt)";
    }
}

//读入零件
int dataReader::loadParts()
{
    cout << "\nstart load parts data" << endl;
    //查找零件文件
    std::ifstream partFile;
    for (auto &file : partFileNames)
    {
        partFile.open(file, std::ios::in);
        if (partFile.good())
            break;
    }
    //未找到
    if (partFile.bad())
    {
        throw runtime_error("not found input file part.tbl");
    }
    int cnt = 0;
    std::string s;

    auto st = clock();
    while (getline(partFile, s))
    {
        cnt++;
        //显示进度条
        if (cnt % 200000 == 0)
        {
            cout << Utils::getTag(cnt / 200000) << endl;
        }
        // 4个实例读入不同数据
        if (cnt % totalProgramCount + 1 != index)
            continue;
        //调用工具类解析
        auto part = Utils::loadPart(s);
        //插入
        partIdHash[part.pPartkey] = parts.size();
        parts.emplace_back(part);
    }
    auto ed = clock();
    //输出
    cout << "\n-------------------------\n";
    cout << "load parts data over.\n";
    cout << "time cost:" << (double)(ed - st) / CLOCKS_PER_SEC << "s.\n";
    cout << "load " << parts.size() << " lines\n\n";

    return 0;
}

int dataReader::loadOrders()
{
    cout << "\nstart load orders data" << endl;
    //查找订单文件
    std::ifstream partFile;
    for (auto &file : orderFileName)
    {
        partFile.open(file, std::ios::in);
        if (partFile.good())
            break;
    }
    //未找到
    if (partFile.bad())
    {
        throw runtime_error("not found input file lineitem.tbl");
    }
    int cnt = 0;
    std::string s;
    auto st = clock();
    while (getline(partFile, s))
    {
        cnt++;
        //显示进度条
        if (cnt % 6000000 == 0)
        {
            cout << Utils::getTag(cnt / 6000000) << endl;
        }
        // 4个实例读入不同数据
        if (cnt % totalProgramCount + 1 != index)
            continue;
        //调用工具类解析
        auto zipOrder = Utils::loadOrder(s);
        //插入
        orders.emplace_back(zipOrder);
    }
    auto ed = clock();
    //输出
    cout << "\n-------------------------\n";
    cout << "load orders data over.\n";
    cout << "load time cost:" << (double)(ed - st) / CLOCKS_PER_SEC << "s.\n";
    cout << "load " << orders.size() << " lines\n\n";
    return 0;
}
//插入零件，用于server调用
// return false: 已经存在
//       true:  插入成功
bool dataReader::insertPart(ZipPart &part)
{
    //找到了，不用再插入了，返回
    if (partIdHash.count(part.pPartkey))
        return false;
    //插入
    partIdHash[part.pPartkey] = parts.size();
    parts.emplace_back(part);
    return true;
}
