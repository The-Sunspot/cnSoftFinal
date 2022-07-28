//
// Created by 光球层上的黑子 on 2022/6/24.
//

#include "../head/Calculator.h"
#include <iomanip>
#include <mutex>
#include <shared_mutex>

//构造函数
Calculator::Calculator(TotalData &totalData, dataReader &reader, client *sendMessager)
    : totalData(totalData), reader(reader), sendMessager(sendMessager)
{
    index = reader.index;
    totalProgramCount = reader.totalProgramCount;
    is_finished = false;
    is_notify = false;
};
//读取数据，并记录时间
void Calculator::loadDatas()
{
    auto be = std::chrono::steady_clock::now();
    auto begin = clock();
    this->reader.loadOrders();
    this->reader.loadParts();
    auto end = clock();
    auto ed = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(ed - be);
    cout << "load module time cost: " << time_span.count() << "s." << endl;
}

//对一个订单进行处理
void Calculator::solveOrder(ZipOrder &order)
{
    //当前订单中的零件信息不在当前进程的零件表中，则将信息传递给下一个进程
    if (!reader.partIdHash.count(order.lPartkey))
    {
        string ss = Utils::zipOrderToString(order);
        message msg(message::headEnum::order, ss.c_str());
        sendMessager->send(msg);
        return;
    }
    //找到这个订单的零件对应的品牌
    int idHash = reader.partIdHash[order.lPartkey];
    std::pair<int, int> brand = reader.parts[idHash].pBrandCode;

    //找到这个零件品牌对应的统计数据
    auto &targetBrandData = totalData.datas[brand.first - 1][brand.second - 1];
    string container=reader.parts[reader.partIdHash[order.lPartkey]].pContainer;
    targetBrandData->updateData(order,container);
    totalData.addLocalTotalSales(order.lQuantity);
}
//计算主控类
void Calculator::doCalculate(condition_variable &cv, unique_lock<mutex> &lck)
{
    auto chroneBegin = std::chrono::steady_clock::now();
    cout << "calculate start" << endl;
    auto begin = clock();
    int cnt = 0;
    //处理本地订单
    for (auto &order : this->reader.orders)
    {
        cnt++;
        solveOrder(order);
        if (cnt % 10000 == 0)
        {
            //避免发送过快，进行等待
            this_thread::sleep_for(std::chrono::milliseconds(300));
            //打印进度条
            if (cnt % 1500000 == 0)
            {
                cout << Utils::getTag(cnt / 1500000) << endl;
            }
        }
    }
    cout << endl;
    //等待进程全部处理完成
    doAsync(cv, lck);
    //统计本地信息
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            totalData.localPartCount += totalData.datas[i][j]->parts.size();
    //更新
    totalData.addPartCount(totalData.localPartCount);
    totalData.addTotalSales(totalData.localSales);
    //传递统计信息
    this->sendTotalData(this->sendMessager->localServerPort,
                        totalData.localPartCount,
                        totalData.localSales);
    //等待零件信息传递完毕
    doAsync(cv, lck);

    //输出
    double avg = (totalData.totalSales + 0.) / totalData.partCount;
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "totalSales:" << totalData.totalSales << std::endl;
    std::cout << "totalPartCount:" << totalData.partCount << std::endl;
    std::cout << "avg:" << avg << std::endl;
    //根据统计信息记录不畅销产品，并通信
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            for (auto &k : totalData.datas[i][j]->parts)
            {
                if (k.saleCount < avg * 0.3)
                {
                    sendMessager->send(message(message::headEnum::underSale, Utils::zipPartData(k).c_str()));
                }
            }
        }
    }
    //等待通信完毕
    doAsync(cv, lck);
    //关闭连接
    sendMessager->close();
    //输出
    totalData.outputUnderSale();
    auto end = clock();

    // cout << "calculate module time cost:" << (double)(end - begin) / TIME_TRANS << "s.\n";

    auto chroneEnd = std::chrono::steady_clock().now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(chroneEnd - chroneBegin);
    cout << "calculate module time cost: " << time_span.count() << endl;
}
//尝试插入不畅销订单
// true插入成功，否则失败
bool Calculator::tryInsertUndersalePart(PartData part)
{
    return totalData.tryInsertUnderSale(part);
}

//转发信息
void Calculator::transferUndersalePartData(message m)
{
    sendMessager->send(m);
}
//修改flag量
void Calculator::setFinishAndNotify(bool f, bool t)
{
    lock_guard lg(mtx);
    is_finished = f;
    is_notify = t;
}
//修改flag量
void Calculator::setNotify(bool t)
{
    lock_guard lg(mtx);
    is_notify = t;
}

//传递结束信息
void Calculator::sendFinish(const char *cnt)
{
    char temp[6];
    strcpy(temp, cnt);
    temp[index]++;
    sendMessager->send(message(message::headEnum::finish, temp));
}
//检查是否全部结束
bool Calculator::checkFinish(const char *cnt) const
{
    for (int i = 1; i <= totalProgramCount; i++)
        if (cnt[i] < '2')
            return false;
    return true;
}
//发送统计信息
void Calculator::sendTotalData(int port, int partCount, long long totalSale) const
{
    auto s = Utils::zipTotalData(port, partCount, totalSale);
    sendMessager->send(message(message::headEnum::totalData, s.c_str()));
}
//传递统计信息
void Calculator::transferTotalData(const message &m) const
{
    sendMessager->send(m);
}
//发送零件信息
void Calculator::sendPart(ZipPart &part)
{
    sendMessager->sendPart(part);
}
//进程间同步
void Calculator::doAsync(condition_variable &cv, unique_lock<mutex> &lck)
{
    sendMessager->runIos();
    sendMessager->resetIos();
    setFinishAndNotify(true, false);
    sendFinish();
    cv.wait(lck);
    sendMessager->runIos();
    sendMessager->resetIos();
    setFinishAndNotify(false, true);
}