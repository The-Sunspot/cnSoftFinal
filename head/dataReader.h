//
// Created by 光球层上的黑子 on 2022/6/15.
//

#ifndef CNSOFTBEI_DATAREADER_H
#define CNSOFTBEI_DATAREADER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "../head/ZipPart.h"
#include "ZipOrder.h"

class dataReader
{
private:
    //零件表可能位置
    std::vector<std::string> partFileNames{
            R"(../data/part.tbl)",
            R"(data/part.tbl)",
            R"(part.tbl)"
    };
    //订单表可能位置
    std::vector<std::string> orderFileName{
            R"(../data/lineitem.tbl)",
            R"(data/lineitem.tbl)",
            R"(lineitem.tbl)"
    };
public:
    //零件数组
    std::vector<ZipPart> parts;
    //零件hash，用于建立映射以便访问
    std::unordered_map<int, int> partIdHash;
    //订单数组
    std::vector<ZipOrder> orders;
    //当前程序编号和程序实例总数
    int index, totalProgramCount;

    //插入零件，用于server调用
    //return false: 已经存在
    //       true:  插入成功
    bool insertPart(ZipPart &part);
    dataReader(int index, int n, bool isTesting = false) ;
    int loadParts();  //获取Part表数据
    int loadOrders(); //获取lineitem表数据
};

#endif // CNSOFTBEI_DATAREADER_H
