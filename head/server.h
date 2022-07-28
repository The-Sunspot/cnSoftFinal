//
// Created by 光球层上的黑子 on 2022/7/1.
//

#ifndef CNSOFTBEI_SERVER_H
#define CNSOFTBEI_SERVER_H

#include <iostream>
#include "asio.hpp"
#include "message.h"
#include "Calculator.h"
using namespace asio;
using namespace std;
class server
{
private:
    //端口
    int port;
    // io 服务
    io_service ios;
    // socket部分
    ip::tcp::socket sock;
    ip::tcp::acceptor acceptor;

    //缓冲部分
    static const int bufferSize = 84;
    char msgFromClient[bufferSize]{};
    char msgToClient[bufferSize]{};

    //统计类的指针
    TotalData &totalData;

public:
    //构造函数，会自动开启新线程建立连接并阻塞等待
    server(int p, condition_variable &cv, Calculator &calculator, TotalData &data);
};

#endif // CNSOFTBEI_SERVER_H
