//
// Created by 光球层上的黑子 on 2022/7/2.
//

#include "../head/client.h"

client::client(int p, int localPort) : connectServerPort(p), localServerPort(localPort), sock(ios), ep(ip::address::from_string("127.0.0.1"), p)
{
    try
    {
        sock.connect(ep);
        cout << "connect to the server: " << ep << endl;
    }
    catch (const std::exception &e)
    {
        cout << "connect fail!" << endl;
        std::cerr << e.what() << '\n';
    }
}

void client::close()
{
    sock.close();
    ios.stop();
}
void client::sendConnect()
{
    this->send(message(message::headEnum::connect));
}
void client::sendLoad()
{
    this->send(message(message::headEnum::load));
}
void client::sendStart(int idx)
{
    this->send(message(message::headEnum::start, to_string(idx).c_str()));
}
void client::sendPart(ZipPart &zippart)
{
    send(message(message::headEnum::part, Utils::zipPartToString(zippart).c_str()));
}

void client::send(const string &s)
{
    unique_lock lg(m_mtx);
    if (s.size() > bufferSize)
    {
        cout << "to large msg" << endl;
    }
    strcpy(msgToServer, s.c_str());
    asio::async_write(sock, asio::buffer(msgToServer), [](const asio::error_code &ec, std::size_t bytes_transferred) {});
}
void client::send(const message &msg)
{
    unique_lock lg(m_mtx);
    strcpy(msgToServer, msg.getSendMsg());
    asio::async_write(sock, asio::buffer(msgToServer), [](const asio::error_code &ec, std::size_t bytes_transferred) {});
}