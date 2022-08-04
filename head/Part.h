//
// Created by 光球层上的黑子 on 2022/6/12.
//

#ifndef CNSOFTBEI_PART_H
#define CNSOFTBEI_PART_H

#include <string>
//数据库中的Part对象，实际不使用
class Part {
public:
    int pPartkey;
    std::string pName;
    std::string pMfgr;
    std::string pBrand;
    std::string pType;
    int pSize;
    std::string pContainer;
    std::string pRetailprice;
    std::string pComment;
};


#endif //CNSOFTBEI_PART_H
