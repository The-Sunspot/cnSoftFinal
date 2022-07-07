//
// Created by 光球层上的黑子 on 2022/7/2.
//

#ifndef CNSOFTBEI_CLIENT_H
#define CNSOFTBEI_CLIENT_H
#include <iostream>
#include <mutex>
#include "asio.hpp"
#include "message.h"
#include "ZipPart.h"
#include "Utils.h"

using namespace asio;
using namespace std;
class client
{
private:
    // io 服务
    io_service ios;
    // socket部分
    ip::tcp::socket sock;
    ip::tcp::endpoint ep;
    //缓冲部分
    static const int bufferSize = 64;
    char msgToServer[bufferSize];
    char msgFromServer[bufferSize];
    // send同步锁
    std::mutex m_mtx;

public:
    //端口信息
    int connectServerPort;
    int localServerPort;

    //关闭
    void close();
    //处理化，自动建立连接
    client(int p, int localPort);
    //异步发送
    void send(const string &s);
    void send(const message &msg);

    void sendConnect();
    void sendLoad();
    void sendStart();
    void sendPart(ZipPart &zippart);
    //调用ios.run
    void runIos(){ios.run();}
    //调用ios.reset
    void resetIos(){ios.reset();}
};

#endif // CNSOFTBEI_CLIENT_H
