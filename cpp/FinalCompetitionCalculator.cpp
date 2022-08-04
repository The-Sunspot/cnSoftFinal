//
// Created by 光球层上的黑子 on 2022/8/4.
//
#include <iomanip>
#include "../head/FinalCompetitionCalculator.h"

void FinalCompetitionCalculator::doCalculate(condition_variable &cv, unique_lock<std::mutex> &lck)
{
    auto be = std::chrono::steady_clock::now();
    // auto calculateBegin = std::chrono::steady_clock::now();

    //本地预处理
    {
        //计时
        auto preprocessBegin = std::chrono::steady_clock::now();

        cout << "preprocessing.." << endl;
        this->preprocess(cv, lck);

        auto preprocessEnd = std::chrono::steady_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(preprocessEnd - preprocessBegin);
        cout << "preprocess time cost: " << time_span.count() << "s.\n";
    }
    //通信零件销售信息
    {

        //计时
        auto comPartBegin = std::chrono::steady_clock::now();

        cout << "communicating.." << endl;
        this->communicatePartSale(cv, lck);

        auto comPartEnd = std::chrono::steady_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(comPartEnd - comPartBegin);
        cout << "first communicating time cost: " << time_span.count() << "s.\n";
    }
    //本地统计不畅销
    {

        //计时
        auto localBegin = std::chrono::steady_clock::now();

        cout << "local calculating.." << endl;
        this->getLocalUnderSale(cv, lck);

        auto lcoalEnd = std::chrono::steady_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(lcoalEnd - localBegin);
        cout << "local calculate time cost: " << time_span.count() << "s.\n";
    }
    //发送答案
    {

        //计时
        auto mergeAnsBegin = std::chrono::steady_clock::now();

        cout << "merging.." << endl;
        this->mergeQueryAns(cv, lck);

        auto mergeAnsEnd = std::chrono::steady_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(mergeAnsEnd - mergeAnsBegin);
        cout << "merge time cost: " << time_span.count() << "s.\n";
    }

    auto ed = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(ed - be);
    cout << "calculate module time cost:" << time_span.count() << "s.\n";
}

void FinalCompetitionCalculator::localCalculate()
{
}
//预处理
//将有用的订单、零件缓存上。
//把本地订单统计到零件数据上
void FinalCompetitionCalculator::preprocess(condition_variable &cv, unique_lock<std::mutex> &lck)
{
    //遍历零件，通信信息
    for (auto &part : this->reader.parts)
    {
        if (part.pContainer == this->targetContainer && part.pBrandCode == this->targetBrand)
        {
            // partKeyMap[part.pPartkey] = 1;
            addPartKey(part.pPartkey);
            sendMessager->send(message(message::partKey, to_string(part.pPartkey).c_str()));
            this->data.insertPartData({part.pPartkey, 0, 0});
        }
    }
    doAsync(cv, lck);
    // cout << "partkeymap size:" << partKeyMap.size() << endl;
    //遍历订单，进行统计
    for (auto &order : this->reader.orders)
    {
        if (partKeyMap.count(order.lPartkey))
        {
            this->orders.emplace_back(order); //将要处理的订单加入到容器中，可以便于之后要进行的不畅销订单统计
            this->data.solveAnOrder(order);   //将订单销售信息统计到对应的零件汇总信息中
        }
    }
    doAsync(cv, lck);
    // this_thread::sleep_for(std::chrono::milliseconds(1000));
}

//第一次通信，目的是把每个零件的订单数和总销量在四个进程同步，这样才能算平均值
void FinalCompetitionCalculator::communicatePartSale(condition_variable &cv, unique_lock<std::mutex> &lck)
{
    int cnt = 0;
    //遍历零件汇总信息，发送
    for (auto &i : data.partData)
    {
        sendMessager->sendPartSaleData(i);
        cnt++;
        // if (cnt % 100 == 0)
        //     this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    //降低send速度，减轻io压力
    // this_thread::sleep_for(std::chrono::milliseconds(200));
    //等待通信完成
    doAsync(cv, lck);
    this->data.mergeData();
    // this_thread::sleep_for(std::chrono::milliseconds(1000));
}

//第二次本地计算，在同步之后有了完整的零件销售情况，就可以算平均值了
//本地算平均值，之后就可以根据平均值统计每个零件的不畅销销售额
void FinalCompetitionCalculator::getLocalUnderSale(condition_variable &cv, unique_lock<std::mutex> &lck)
{
    data.setUnderSales(0.);
    int cnt = 0;
    for (auto &order : this->orders)
    {
        auto partSaleData = data.partData[this->data.partidHash[order.lPartkey]];
        long double avg = partSaleData.getAvg();
        // cout<<"partid: "<<order.lPartkey<<" avg:"<<avg<<endl;
        // cnt++;
        // if (cnt % 100 == 0)
        // {
        //     this_thread::sleep_for(std::chrono::milliseconds(200));
        // }
        if (order.lQuantity < avg * 0.3)
        {
            this->data.addUnderSales(order.lExtendedprice);
        }
    }
    doAsync(cv, lck);
    // this_thread::sleep_for(std::chrono::milliseconds(1000));
}

//最后通信，就传一个double就行
void FinalCompetitionCalculator::mergeQueryAns(condition_variable &cv, unique_lock<std::mutex> &lck)
{
    // 4->3->2->1这样通信
    for (int now = select_idx - 1; now != select_idx; now = now - 1 == 0 ? totalProgramCount : now - 1)
    {
        if (now == 0)
            now = totalProgramCount;
        //应该自己发送
        if (this->index == now)
        {
            cout << "this program is sending...\n"
                 << endl;
            // cout<<data.underSales<<endl;
            sendMessager->sendFinalAns(data.underSales);
        }
        //不是，则等待
        else
        {

            cout << "waiting " << now << " sending data...\n";
        } //稍作等待
        // this_thread::sleep_for(std::chrono::milliseconds(1000));
        doAsync(cv, lck);
    }
    doAsync(cv, lck);
    // this_thread::sleep_for(std::chrono::milliseconds(1000));
    cout << fixed << setprecision(10)
         << this->data.underSales / 7 << endl;
}
