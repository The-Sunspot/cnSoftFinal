//
// Created by 光球层上的黑子 on 2022/6/15.
//

#ifndef CNSOFTBEI_UTILS_H
#define CNSOFTBEI_UTILS_H

#include <vector>
#include <string>
#include <climits>
#include "Part.h"
#include "ZipPart.h"
#include "ZipOrder.h"
#include "PartData.h"
#include "PartSaleData.h"

class Utils
{
public:

    const static std::vector<std::string>  containers;

    //分割字符串，传入分隔符，类似于python的split
    std::vector<std::string> static split(const std::string &str, char ch = ' ');

    //解析从数据中读入的一行零件
    ZipPart static loadPart(const std::string &line);
    ZipPart static loadPart(const char *line);

    //解析从数据中读入的一行订单
    ZipOrder static loadOrder(const std::string &line);
    ZipOrder static loadOrder(const char *line);

    //序列化  zipOrder->string
    std::string static zipOrderToString(const ZipOrder &zipOrder);
    //序列化  zipPart->string
    std::string static zipPartToString(const ZipPart &zipPart);
    //序列化  totalData(data part)->string
    std::string static zipTotalData(int from, int partCount, long long totalSale);
    //序列化  PartData->string
    std::string static zipPartData(const PartData &part);

    //反序列化 char*->PartData
    PartData static unzipPartData(const char *line);
    //反序列化 string->PartData
    PartData static unzipPartData(const std::string &line);

    //反序列化 char*->PartData
    ZipOrder static unZipOrderString(const char *line);
    //反序列化 string->PartData
    ZipOrder static unZipOrderString(const std::string &line);

    //反序列化 string->ZipPart
    ZipPart static unZipPartString(const std::string &line);
    //反序列化  char *->ZipPart
    ZipPart static unZipPartString(const char *line);

    std::string static zipStart(int idx,const std::string& container,std::pair<int,int>brand);

    std::tuple<int,std::string,std::pair<int,int>> static unZipStart(const std::string &s);
    std::tuple<int,std::string,std::pair<int,int>> static unZipStart(const char *s);
    //反序列化 string->pair<id,<count,sales>>
    std::pair<int, std::pair<int, int>> static unZipTotalData(const std::string &data);
    //反序列化 char*->pair<id,<count,sales>>
    std::pair<int, std::pair<int, int>> static unZipTotalData(const char *line);

    static std::string zipPartSaleDataToString(const PartSaleData partSaleData, int from);

    static std::pair<PartSaleData,int> unzipPartSaleData(const std::string &s);
    static std::pair<PartSaleData,int> unzipPartSaleData(const char *s);

    //返回一个string，作为进度条
    // x取0-10的整数
    static std::string getTag(int x);

    //检查输入的select语句是否合法，
    //非法格式返回{-1,-1}，非法数字返回{INT_MAX，INT_MAX}，合法格式返回数字{ch1-'0',ch2-'0'}
    static std::pair<int, int> judgeInput(const std::string &s);

    static std::tuple<std::string,std::pair<int,int>,std::string> judgeInputExtended(const std::string &s);


    static std::string getRandomContainer();
    static std::pair<int,int> getRandomBrand();

    static std::tuple<bool,std::pair<int,int>,std::string> dealInputParm(const std::string &s);
};

#endif // CNSOFTBEI_UTILS_H