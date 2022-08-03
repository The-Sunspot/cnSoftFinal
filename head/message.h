//
// Created by 光球层上的黑子 on 2022/7/2.
//

#ifndef CNSOFTBEI_MESSAGE_H
#define CNSOFTBEI_MESSAGE_H
#include <cstring>
#include <string>

using std::string;
class message
{
private:
    static const int MAX_HEAD_LENGTH = 1;
    static const int MAX_BODY_LENGTH = 82;
    char head[MAX_HEAD_LENGTH + 1];                      //头
    char body[MAX_BODY_LENGTH + 1];                      //体
    char sendMsg[MAX_HEAD_LENGTH + MAX_BODY_LENGTH + 1]; //总

public:
    enum headEnum
    {
        connect,   //连接同步头
        load,      //加载同步头
        start,     //计算同步头
        order,     //订单头
        finish,    // finish同步头
        underSale, //不畅销零件头
        totalData, //统计信息头
        part,      //零件头
        partData,  //零件信息头
        finalPart, //决赛————订单销售头
        finalAns,  //决赛————查询结果头
        unknown    //未知情况
    };
    message(headEnum head, const char *body);
    message(headEnum head);
    message(const char *s);
    //获取头字符串，一般不使用
    const char *const getHead() const;
    //获取信息头--枚举类型
    headEnum getHeadEnum();
    //获取体
    const char *const getBody() const;
    //获取用于传递的整串
    const char *const getSendMsg() const;
    //设置信息体
    int setBody(const char *s);
    //设置信息头
    void setHead(headEnum e);
};
#endif // CNSOFTBEI_MESSAGE_H
