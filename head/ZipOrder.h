//
// Created by 光球层上的黑子 on 2022/6/15.
//

#ifndef CNSOFTBEI_ZIPORDER_H
#define CNSOFTBEI_ZIPORDER_H


#include <string>

//压缩存储的零件信息
class ZipOrder {
public:
    //零件号
    int lPartkey;
    //销售量
    int lQuantity;
    //销售额
    double lExtendedprice;
};


#endif //CNSOFTBEI_ZIPORDER_H
