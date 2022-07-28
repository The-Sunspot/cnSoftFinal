//
// Created by 光球层上的黑子 on 2022/7/5.
//

#include <iomanip>
#include "../head/AdvancedCalculator.h"
//改进版计算
void AdvancedCalculator::doCalculate(condition_variable &cv, unique_lock<std::mutex> &lck)
{
    auto be = std::chrono::steady_clock::now();
    // send part
    auto calculateBegin = std::chrono::steady_clock::now();

    //核心计算算法第一步
    {
        //计时
        auto partTabelCommunicateBeginTime = std::chrono::steady_clock::now();

        cout << "sending parts..." << endl;
        partTabelCommunicate(cv, lck);

        auto partTabelCommunicateEndTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(partTabelCommunicateEndTime - partTabelCommunicateBeginTime);
        cout << "send parts time cost: " << time_span.count() << "s.\n";
    }

    //核心算法第二步
    {
        //计时
        auto localCalculateBeginTime = std::chrono::steady_clock::now();
        cout << "local calculating...\n";
        localCalculate(cv, lck);

        auto localCalculateEndTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(localCalculateEndTime - localCalculateBeginTime);
        cout << "local calculate time cost: " << time_span.count() << "s.\n";
    }

    //核心算法第三步，零件通信
    {
        //计时
        auto partDataCommunicateBeginTime = std::chrono::steady_clock::now();
        cout << "sending calculated datas...\n";
        partDataCommunicate(cv, lck);

        auto partDataCommunicateEndTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(partDataCommunicateEndTime - partDataCommunicateBeginTime);
        cout << "send calculated data time cost: " << time_span.count() << "s.\n";
    }

    //主进程输出

    if (this->index == 1)
    {
        getTotalDatas();
        totalData.calculateUnderSale();
    }
    else
    {
        sendMessager->close();
    }
    sendMessager->close();

    auto ed = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(ed - be);
    cout << "calculate module time cost:" << time_span.count() << "s.\n";
}
//改进版订单处理
void AdvancedCalculator::solveOrder(ZipOrder &order)
{

    //找到这个订单的零件对应的品牌
    int idHash = reader.partIdHash[order.lPartkey];
    std::pair<int, int> brand = reader.parts[idHash].pBrandCode;

    //找到这个零件品牌对应的统计数据
    auto &targetBrandData = totalData.datas[brand.first - 1][brand.second - 1];

    targetBrandData->updateData(order);
    totalData.addLocalTotalSales(order.lQuantity);
}

//计算totalData中的销售总额
void AdvancedCalculator::getTotalDatas()
{
    totalData.setTotalSales(0);
    //遍历品牌
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            auto &brandData = this->totalData.datas[i][j];
            //遍历商品
            for (auto &part : brandData->parts)
            {
                totalData.addTotalSales(part.saleCount);
            }
        }
    }
}

//核心计算算法第一步
//首先各个进程间发送零件信息，使得每一个进程都拥有完整的零件表
//这样可以保证所有进程的订单都可以在本地进行处理，而无需通信
//因为订单的量远大于零件的量，因此这么操作会使得通信量降到最低
//这也是改进版计算类和普通计算类的核心思路差距
void AdvancedCalculator::partTabelCommunicate(condition_variable &cv, unique_lock<std::mutex> &lck)
{

    int sz = reader.parts.size();
    //首先将所有零件同步
    for (int i = 0; i < sz; i++)
    {
        sendMessager->sendPart(reader.parts[i]);
        if (i % 10000 == 0)
        {
            //降低send速度，减轻io压力
            //可以根据具体环境配置进行调整
            this_thread::sleep_for(std::chrono::milliseconds(200));

            //进度条
            if (i % 50000 == 0)
                cout << Utils::getTag(i / 50000) << endl;
        }
    }
    //降低send速度，减轻io压力
    this_thread::sleep_for(std::chrono::milliseconds(500));
    //等待通信完成
    doAsync(cv, lck);

    //统计
    totalData.setLocalPartCount(reader.parts.size());
    totalData.setPartCount(reader.parts.size());
}

//核心算法第二步，本地计算
//遍历本地所有订单，使用totalData的相关方法进行本地计算
//是目前最耗时的一部分
void AdvancedCalculator::localCalculate(condition_variable &cv, unique_lock<std::mutex> &lck)
{
    //遍历所有订单
    for (auto &order : this->reader.orders)
        solveOrder(order);
    doAsync(cv, lck);
}

//核心算法第三步，零件通信
//各个进程有完整的零件表，但是只有一部分订单表
//因此其统计的数据也是一部分
//所以这里需要将所有的数据进行汇总
//汇总方式是从最后一个进程开始，依次向连接的进程发送信息，直到发送到输出进程为止
//相比较于都向输出进程发送信息，两者通信量一致，但是如此操作，服务器的io负担较小，代价是降低了一些性能。
void AdvancedCalculator::partDataCommunicate(condition_variable &cv, unique_lock<std::mutex> &lck)
{

    // 4->3->2->1这样通信
    int sendCount = 0;
    for (int now = this->totalProgramCount; now != 1; now--)
    {
        //应该自己发送，就遍历发送
        if (this->index == now)
        {
            cout << "this program is sending...\n"
                 << endl;
            //遍历品牌
            for (int i = 0; i < 5; i++)
            {
                for (int j = 0; j < 5; j++)
                {
                    //遍历零件
                    for (auto &k : totalData.datas[i][j]->parts)
                    {
                        sendCount++;
                        //进度条
                        if (sendCount % 200000 == 0)
                        {
                            cout << Utils::getTag(sendCount / 200000) << endl;
                        }
                        //发送
                        sendMessager->send(message(message::headEnum::partData, Utils::zipPartData(k).c_str()));
                    }
                }
            }
        }
        //不是，则等待
        else
        {
            //稍作等待
            this_thread::sleep_for(std::chrono::milliseconds(2000));
            cout << "waiting " << now << " sending data...\n";
        }
        doAsync(cv, lck);
    }
    doAsync(cv, lck);
}