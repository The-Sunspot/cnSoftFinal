//
// Created by 光球层上的黑子 on 2022/6/15.
//

#include<sstream>
#include <tuple>
#include <iostream>
#include <string>
#include <random>
#include <ctime>
#include "../head/Utils.h"
//分割字符串，传入分隔符，类似于python的split
std::vector<std::string> Utils::split(const std::string &str, const char ch)
{
    std::vector<std::string> rec;
    std::string tmp;
    bool inContainer=false;
    for (auto &i : str)
    {
        if(i=='\''){
            if(inContainer) inContainer= false;
            else    inContainer=true;
        }
        else if (i == ch)
        {
            if(inContainer) tmp+=i;
            else if (tmp.length() >= 1)
            {
                rec.emplace_back(tmp);
                tmp.clear();
            }
        }
        else
        {
            tmp += i;
        }
    }
    if(!tmp.empty())  rec.push_back(tmp);
    return rec;
}

//解析从数据中读入的一行零件
ZipPart Utils::loadPart(const std::string &line)
{
    return Utils::loadPart(line.c_str());

}

//解析从数据中读入的一行订单
ZipOrder Utils::loadOrder(const std::string &line)
{
    return Utils::loadOrder(line.c_str());
}

//解析从数据中读入的一行订单
ZipOrder Utils::loadOrder(const char *line)
{
    ZipOrder zipOrder{0, 0, 0};
    int cnt = 0;
    double base = 1;
    for (int j = 0;; j++)
    {
        char i = line[j];
        if (i == '\0')
            throw std::runtime_error{std::string("error in ZipOrder Utils::loadOrder(const char *line). error: string end before unZip")};
        if (i == '|')
        {
            if (cnt == 5)
                break;
            cnt++;
        }
        else
        {
            if (cnt == 1)
                zipOrder.lPartkey = zipOrder.lPartkey * 10 + i - '0';
            else if (cnt == 4)
                zipOrder.lQuantity = zipOrder.lQuantity * 10 + i - '0';
            else if (cnt == 5)
            {
                if (i == '.')
                {
                    base *= 0.1;
                    continue;
                }
                if (base >= 1)
                    zipOrder.lExtendedprice = zipOrder.lExtendedprice * 10 + i - '0';
                else
                    zipOrder.lExtendedprice += base * (i - '0');
                if (base < 1)
                    base *= 0.1;
            }
        }
    }
    return zipOrder;
}

//解析从数据中读入的一行零件
ZipPart Utils::loadPart(const char *line)
{
    ZipPart zipPart = {0, {0, 0},""};
    int cnt = 0;
    for (int j = 0;; j++)
    {
        char i = line[j];
        if (i == '|')
            cnt++;
        else
        {
            if (cnt == 0)
                zipPart.pPartkey = zipPart.pPartkey * 10 + i - '0';
            else if (cnt == 3)
            {
                if (i >= '0' && i <= '9')
                {
                    if (!zipPart.pBrandCode.first)
                        zipPart.pBrandCode.first = i - '0';
                    else
                    {
                        zipPart.pBrandCode.second = i - '0';
                        //break;
                    }
                }
            }
            else if(cnt == 6){
                while(line[j]!='|'){
                    zipPart.pContainer+=line[j];
                    j++;
                }
                break;
            }
        }
    }
    return zipPart;
}
//序列化  zipOrder->string
std::string Utils::zipOrderToString(const ZipOrder &zipOrder)
{
    return std::to_string(zipOrder.lPartkey) + '|' + std::to_string(zipOrder.lQuantity) + '|' + std::to_string(zipOrder.lExtendedprice) + '|';
}
//序列化  zipPart->string
std::string Utils::zipPartToString(const ZipPart &zipPart)
{
    return std::to_string(zipPart.pPartkey) + '|' + std::to_string(zipPart.pBrandCode.first) + '|' + std::to_string(zipPart.pBrandCode.second) + '|' +zipPart.pContainer+'|';
}

//反序列化 char *->ZipPart
ZipOrder Utils::unZipOrderString(const char *line)
{
    ZipOrder zipOrder{0, 0, 0};
    int cnt = 0;
    double base = 1;
    bool dig = false;
    for (int i = 0; cnt < 3; i++)
    {
        if (line[i] == '|')
            cnt++;
        else if (line[i] == '.')
        {
            dig = true;
        }
        else
        {
            if (cnt == 0)
            {
                zipOrder.lPartkey = zipOrder.lPartkey * 10 + line[i] - '0';
            }
            else if (cnt == 1)
            {
                zipOrder.lQuantity = zipOrder.lQuantity * 10 + line[i] - '0';
            }
            else
            {
                if (dig)
                {
                    base /= 10;
                    zipOrder.lExtendedprice += base * (line[i] - '0');
                }
                else
                {
                    zipOrder.lExtendedprice = zipOrder.lExtendedprice * 10 + line[i] - '0';
                }
            }
        }
    }
    return zipOrder;
}

//反序列化 string->ZipPart
ZipOrder Utils::unZipOrderString(const std::string &line)
{
    return Utils::unZipOrderString(line.c_str());
}

//反序列化  string->ZipPart
ZipPart Utils::unZipPartString(const std::string &line)
{
    return Utils::unZipPartString(line.c_str());
}

//反序列化  char *->ZipPart
ZipPart Utils::unZipPartString(const char *line)
{
    ZipPart zipPart = {0, {0, 0},""};
    int cnt = 0;
    for (int j = 0;; j++)
    {
        char i = line[j];
        if (i == '|'){
            cnt++;
            if(cnt==4)  break;
        }
        else
        {
            if (cnt == 0)
                zipPart.pPartkey = zipPart.pPartkey * 10 + i - '0';
            else if (cnt == 1)
            {
                zipPart.pBrandCode.first = i - '0';
            }
            else if (cnt == 2)
            {
                zipPart.pBrandCode.second = i - '0';
            }
            else if(cnt == 3){
                zipPart.pContainer += i;
            }
        }
    }
    return zipPart;
}

std::string Utils::zipTotalData(int from,int partCount, long long totalSale) {
    return std::to_string(from)+'|'
    +std::to_string(partCount)+'|'
    +std::to_string(totalSale)+'|';
}
//反序列化 string->pair<id,<count,sales>>
std::pair<int,std::pair<int,int>> Utils::unZipTotalData(const std::string &data) {
    return Utils::unZipTotalData(data.c_str());
}

//反序列化 string->pair<id,<count,sales>>
std::pair<int,std::pair<int,int>> Utils::unZipTotalData(const char *line) {
    std::pair<int,std::pair<int,int>>rec{0,{0,0}};
    int cnt = 0;
    for (int i = 0; cnt < 3; i++)
    {
        if (line[i] == '|')
            cnt++;
        else
        {
            if (cnt == 0)
            {
                rec.first=rec.first*10+line[i]-'0';
            }
            else if(cnt==1)
            {
                rec.second.first=rec.second.first*10+line[i]-'0';
            }
            else{
                rec.second.second=rec.second.second*10+line[i]-'0';
            }
        }
    }
    return rec;
}

//序列化  PartData->string
std::string Utils::zipPartData(const PartData &part) {
    return
        std::to_string(part.id)+'|'
        +std::to_string(part.brandCode.first)
        +std::to_string(part.brandCode.second)
        +std::to_string(part.saleCount)+'|'
        +std::to_string(part.totalSales)+'|'
        +part.container;
}

//反序列化 char*->PartData
//id|12xxxxx|yyyyy|zzzz
PartData Utils::unzipPartData(const char *line) {
    int cnt=0;
    double base=1;
    bool dig=false;
    PartData part{};
    for(int i=0;line[i]!='\0';i++) {
        if (line[i] == '|') cnt++;
        else if (line[i] == '.') dig = true;
        else {
            if (cnt == 0) {
                part.id = part.id * 10 + line[i] - '0';
            } else if (cnt == 1) {
                if (!part.brandCode.first) {
                    part.brandCode.first = line[i] - '0';
                } else if (!part.brandCode.second) {
                    part.brandCode.second = line[i] - '0';
                } else {
                    part.saleCount = part.saleCount * 10 + line[i] - '0';
                }
            } else if(cnt==2){
                if (dig) {
                    base /= 10;
                    part.totalSales += (line[i] - '0') * base;
                } else {
                    part.totalSales = part.totalSales * 10 + line[i] - '0';
                }
            }
            else{
                part.container+=line[i];
            }

        }
    }
    return part;
}

//反序列化 string->PartData
PartData Utils::unzipPartData(const std::string &line) {
    return Utils::unzipPartData(line.c_str());
}

//返回一个string，作为进度条
// x取0-10的整数
std::string Utils::getTag(int x) {
    std::string s = "[";
    for (int i = 1; i <= x; i++)
        s += "**";
    for (int i = x; i < 10; i++)
        s += "..";
    s += ']';
    s += std::to_string(x * 10) + "%";
    return s;
}

//检查输入的select语句是否合法，
//非法格式返回{-1,-1}，非法数字返回{INT_MAX，INT_MAX}，合法格式返回数字{ch1-'0',ch2-'0'}
std::pair<int, int> Utils::judgeInput(const std::string &s) {
    //检查ch1和ch2是否为合法数字
    //不为数字返回{-1,-1}，非法数字返回{INT_MAX，INT_MAX}，合法数字返回数字{ch1-'0',ch2-'0'}
    auto checkFunction = [](char ch1, char ch2) -> std::pair<int, int>
    {
        if (std::isdigit(ch1) && std::isdigit(ch2))
        {
            // 12,33这类有效的
            if (ch1 >= '1' && ch1 <= '5' && ch2 >= '1' && ch2 <= '5')
                return {ch1 - '0', ch2 - '0'};
            // 56,01这种无效的
            else
                return {INT_MAX, INT_MAX};
        }
        else
        {
            return {-1, -1};
        }
    };
    //检查s1是否以s2打头，保证输入s1.size()>s2.size();
    //返回bool值
    auto isStartWith = [](const std::string &s1, const std::string &s2) -> bool{
        return (s1.compare(0, s2.size(), s2) == 0);
    };
    //打头的字符串，会被赋值并一起比较
    std::string head;

    //{a}{b}
    if (s.size() == 2)
    {
        return checkFunction(s[0], s[1]);
    }
    // select Brand#{a}{b}
    else if (s.size() == 15)
    {
        head = "select Brand#";
    }
    // select {a}{b}
    else if (s.size() == 9)
    {
        head = "select ";
    }
    //非法长度
    else
    {
        return {-1, -1};
    }
    if (isStartWith(s, head))
        return checkFunction(s[head.size()], s[head.size() + 1]);
    else
        return {-1, -1};
}

std::tuple<std::string, std::pair<int, int>, std::string> Utils::judgeInputExtended(const std::string &s) {
    //检查ch1和ch2是否为合法数字
    //不为数字返回{-1,-1}，非法数字返回{INT_MAX，INT_MAX}，合法数字返回数字{ch1-'0',ch2-'0'}
    auto checkFunction = [](char ch1, char ch2) -> std::pair<int, int>
    {
        if (std::isdigit(ch1) && std::isdigit(ch2))
        {
            // 12,33这类有效的
            if (ch1 >= '1' && ch1 <= '5' && ch2 >= '1' && ch2 <= '5')
                return {ch1 - '0', ch2 - '0'};
                // 56,01这种无效的
            else
                return {INT_MAX, INT_MAX};
        }
        else
        {
            return {-1, -1};
        }
    };
    auto isEqualIgnoreLowerOrUpper=[](const std::string &s1, const std::string &s2)->bool {
        if(s1.size()!=s2.size())    return false;
        for(int i=0;i<s1.size();i++){
            if(std::isalpha(s1[i])&&std::isalpha(s2[i])){
                if(tolower(s1[i])!= tolower(s2[i]))
                    return false;
            }
            else {
                if(s1[i]!=s2[i])
                    return false;
            }
        }
        return true;
    };

    using recType=std::tuple<std::string, std::pair<int, int>, std::string>;
    const recType QUIT={"quit",{0,0},""};
    const recType WRONGINPUT={"wrong",{-1,-1},""};
    const recType RANDOM={"random",{0,0},""};
    const recType INFO={"info",{0,0},""};
    const recType ALL={"all",{0,0},""};
    recType brand={"brand",{0,0},""};
    recType container={"container",{0,0},""};
    recType both={"both",{0,0},""};
    if(s.empty()) return WRONGINPUT;

    std::vector<std::string> inputs=Utils::split(s,' ');

    if(inputs.size()==1){
        if(inputs[0]=="q"||inputs[0]=="quit"){
            return QUIT;
        }
        if(inputs[0]=="random"){
            return RANDOM;
        }
        if(inputs[0]=="info"){
            return INFO;
        }
        if(inputs[0]=="select"||inputs[0]=="all"){
            return ALL;
        }
        return WRONGINPUT;
    }
    else if(inputs.size()==3){
        if(inputs[0]!="select"){
            return WRONGINPUT;
        }
        if(isEqualIgnoreLowerOrUpper(inputs[1],"b")||isEqualIgnoreLowerOrUpper(inputs[1],"brand")){
            if(inputs[2].size()!=2){
                return WRONGINPUT;
            }
            std::get<1>(brand)=checkFunction(inputs[2][0],inputs[2][1]);
            return brand;
        }
        else if(isEqualIgnoreLowerOrUpper(inputs[1],"c")||isEqualIgnoreLowerOrUpper(inputs[1],"container")){
            std::get<2>(container)=inputs[2];
            return container;
        }
        else{
            return WRONGINPUT;
        }
    }
    else if(inputs.size()==5){
        if(inputs[0]!="select"){
            return WRONGINPUT;
        }
        if(
                (isEqualIgnoreLowerOrUpper(inputs[1],"b")||isEqualIgnoreLowerOrUpper(inputs[1],"brand"))
                &&(isEqualIgnoreLowerOrUpper(inputs[3],"c")||isEqualIgnoreLowerOrUpper(inputs[3],"container"))
                ){
            if(inputs[2].size()!=2){
                return WRONGINPUT;
            }
            std::get<1>(both)=checkFunction(inputs[2][0],inputs[2][1]);
            std::get<2>(both)=(inputs[4]);
            return both;
        }
        else if(
                (isEqualIgnoreLowerOrUpper(inputs[1],"c")||isEqualIgnoreLowerOrUpper(inputs[1],"container"))
                &&(isEqualIgnoreLowerOrUpper(inputs[3],"b")||isEqualIgnoreLowerOrUpper(inputs[3],"brand"))
                ){
            if(inputs[4].size()!=2){
                return WRONGINPUT;
            }
            std::get<1>(both)=checkFunction(inputs[4][0],inputs[4][1]);
            std::get<2>(both)=(inputs[2]);
            return both;
        }
        else{
            return WRONGINPUT;
        }
    }
    else{
        return WRONGINPUT;
    }
}

std::string Utils::getRandomContainer() {
    //随机container
    std::vector<std::string>containers={"WRAP JAR","MED DRUM","SM BOX","JUMBO BOX","LG BAG","SM CAN",
                              "MED CAN","WRAP DRUM","LG PACK","SM JAR","WRAP PACK","MED BAG","LG DRUM",
                              "JUMBO JAR","JUMBO PKG","MED PKG","MED CASE","LG BOX","WRAP CAN","JUMBO BAG",
                              "WRAP BOX","SM CASE","LG CASE","LG PKG","LG CAN","MED JAR","LG JAR","JUMBO PACK",
                              "SM DRUM","SM PACK","SM BAG","MED BOX","MED PACK","SM PKG","JUMBO CAN","WRAP BAG",
                              "JUMBO CASE","WRAP PKG","JUMBO DRUM","WRAP CASE"
    };
    //随机数发生器
    std::default_random_engine randomEngine(time(nullptr));
    std::uniform_int_distribution<int> brandGen(1,5);
    std::uniform_int_distribution<int> containerGen(0,containers.size()-1);
    std::pair<int,int> brand={brandGen(randomEngine),brandGen(randomEngine)};
    return containers[containerGen(randomEngine)];
}

std::pair<int, int> Utils::getRandomBrand() {
    std::default_random_engine randomEngine(time(nullptr));
    std::uniform_int_distribution<int> brandGen(1,5);
    return {brandGen(randomEngine),brandGen(randomEngine)};
}

std::string Utils::zipPartSaleDataToString(const PartSaleData partSaleData,int from) {
    return std::to_string(partSaleData.partid)+'|'+ std::to_string(partSaleData.orderCount)+'|'+ std::to_string(partSaleData.totalSale)+'|'+std::to_string(from)+'|';
}

std::pair<PartSaleData,int> Utils::unzipPartSaleData(const std::string &s) {
    return unzipPartSaleData(s.c_str());
}

std::pair<PartSaleData,int> Utils::unzipPartSaleData(const char *s) {
    int cnt=0;
    std::pair<PartSaleData,int> rec={{0,0,0},0};
    for(int i=0;;i++){
        char ch=s[i];
        if(ch=='|'){
            cnt++;
            if(cnt==4)  break;
        }
        else{
            if(cnt==0){
                rec.first.partid=rec.first.partid*10+ch-'0';
            }
            else if(cnt==1){
                rec.first.orderCount=rec.first.orderCount*10+ch-'0';
            }
            else if(cnt==2){
                rec.first.totalSale=rec.first.totalSale*10+ch-'0';
            }
            else{
                rec.second=rec.second*10+ch-'0';
            }
        }
    }
    return rec;
}

