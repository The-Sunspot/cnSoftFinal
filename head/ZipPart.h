//
// Created by 光球层上的黑子 on 2022/6/15.
//

#ifndef CNSOFTBEI_ZIPPART_H
#define CNSOFTBEI_ZIPPART_H


#include <string>

//压缩存储的零件信息
class ZipPart {
public:
    //主键
    int pPartkey;
    //品牌，1~5
    std::pair<int,int> pBrandCode;
};


#endif //CNSOFTBEI_ZIPPART_H
