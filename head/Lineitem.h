//
// Created by 光球层上的黑子 on 2022/6/12.
//

#ifndef CNSOFTBEI_LINEITEM_H
#define CNSOFTBEI_LINEITEM_H

#include<string>
//数据库中的Lineitem对象，实际不使用
class Lineitem{
public:
    int lOrderkey;
    int lPartkey;
    std::string lSuppke;
    int lLinenumber;
    int lQuantity;
    double lExtendedprice;
    double lDdiscount;
    double lTax;
    bool lReturnflag;
    bool lLinestatus;
    std::string lShipdate;
    std::string lCommitdate;
    std::string lReceiptdate;
    std::string lShipinstruct;
    std::string lShipmode;
    std::string lComment;
};
#endif //CNSOFTBEI_LINEITEM_H
