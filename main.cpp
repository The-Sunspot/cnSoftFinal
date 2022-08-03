#include <iostream>
#include <ctime>
#include "head/Part.h"
#include "head/Lineitem.h"
#include "head/dataReader.h"
#include "head/TotalData.h"
#include "head/Calculator.h"
#include "asio.hpp"
#include "head/server.h"
#include "head/client.h"
#include "head/Utils.h"
#include <thread>
#include <string>
#include <mutex>
#include <condition_variable>
#include "head/AdvancedCalculator.h"
#include "head/QueryResponser.h"
#include "head/FinalCompetitionCalculator.h"

using namespace std;
const int serverPort[] = {0, 8001, 8002, 8003, 8004};
#ifdef _WIN64
#define SYS_PAUSE system("pause")
#else
#define SYS_PAUSE pause()
#endif
//#define IS_TEST
#ifdef IS_TEST
#define testflag 1
#else
#define testflag 0
#endif

int main(int argc, char *argv[])
{
    //默认实例数
    int n = 4;
    //命令行参数不全
    if (argc != 2 || (argv[1][0] != '1' && argv[1][0] != '2' && argv[1][0] != '3' && argv[1][0] != '4'))
    {
        cout << "please input program number (1~4)." << endl;
        return -1;
    }
    //过去当前实例标号，1为主进程
    const int index = argv[1][0] - '0';
    cout << "the " << index << "st program launch" << endl;

    //同步锁
    mutex mtx;
    condition_variable cv;
    unique_lock lck(mtx);

    //数据读入类实例
    dataReader reader(index, n, testflag);

    //数据统计类实例
    TotalData totalData;

    //计算类实例
    // AdvancedCalculator为Calculator子类，是演变后的计算类，更快空间更小
    // 详情请见类代码注释
    // Calculator calculator(totalData, reader);
    // AdvancedCalculator calculator(totalData, reader);

    //FinalCompetitionCalculator calculator(totalData, reader);
    //7027145.477142
    FinalCompetitionCalculator calculator({1,1},"WRAP JAR",totalData, reader);
    //接发信息类实例，
    server loadMessager(serverPort[index], cv, calculator, totalData);
    client *sendMessager = nullptr; // 1号实例顺序不同，暂不能赋值

    //查询响应器
    //QueryResponser queryResponser(calculator, totalData);

    string s;

    std::thread interactThread([&]
                               {
        while (getline(cin,s))
        {
            if (!reader.load_flag)
            {
                if (s != "load")
                {
                    cout << "please enter \"load\"!\ninput: ";
                }
                else
                {
                    if (sendMessager == nullptr)
                        sendMessager = new client(serverPort[n], serverPort[index]);
                    //告知
                    sendMessager->sendLoad();
                    // break;
                }
            }
            else if (!calculator.select_flag)
            {
                if (s != "select")
                {
                    cout << "please enter \"select\"!\ninput: ";
                }
                else
                {
                    sendMessager->sendStart(calculator.index);
                    // break;
                }
            }
            else
            {
                //空行处理
                if (s.empty() || s == "\n" || s == "\r\n")
                    continue;
                if(s=="q"||s=="quit"){
                    cout << "goodbye" << endl;
                    cv.notify_one();
                    break;
                }
                //queryResponser.dealInteract(s);
            }
            } });
    interactThread.detach();

    // load
    if (index != 1)
    {
        // cout << "wait for load. input \'load\' in 1st program to load datas." << endl;
        //建立连接
        sendMessager = new client(serverPort[index - 1], serverPort[index]);
        cout << "wait for load. input \'load\' to load datas.\ninput: ";

        cv.wait(lck);
        //告知
        sendMessager->sendLoad();
    }
    else
    { //主进程
        cout << "wait for load. input \'load\' to load datas.\ninput: ";

        //同步
        cv.wait(lck);
        //建立连接
        if (sendMessager == nullptr)
            sendMessager = new client(serverPort[n], serverPort[index]);
        //通知
        sendMessager->sendLoad();
    }

    //设置发送实例
    calculator.sendMessager = sendMessager;
    //读入数据
    calculator.loadDatas();

    // start
    //普通进程无需读入
    if (index != 1)
    {
        // cout << "wait for calculate. input \'select\' in 1st program to do calculate and output." << endl;
        cout << "wait for calculate. input \'select\' to do calculate and output.\ninput: ";

        //等待同步
        cv.wait(lck);
        //通知
        sendMessager->sendStart(calculator.select_idx);
        //计算
        calculator.doCalculate(cv, lck);

        if (calculator.select_idx == calculator.index)
        {
            //queryResponser.printSaleInfo();
            //queryResponser.randomSelect();
            //交互部分
           // cout << R"(calculate over, input 'q' or 'quit' to quit)" << endl;
            //cout
            //    << R"(input 'random' do random query, 'select b/brand {a}{b}' to query brand {a}{b},)" << endl
            //    << R"('select c/container {con}' to query container {con},')" << endl
            //    << R"('select b/brand {a}{b} c/container {con}' or 'select c/container {con} b/brand {a}{b}' to query brand{a}{b} and container {con}.)"
            //    << endl;
            //cout << "input:";
            // interactThread.join();
            cv.wait(lck);
        }
        else
        {
            cout << "calculate over. program exit" << endl;
            SYS_PAUSE;
        }
        return 0;
    }
    else
    { //主进程
        cout << "wait for calculate. input \'select\' to do calculate and output.\ninput: ";

        //同步
        cv.wait(lck);
        //传递
        sendMessager->sendStart(calculator.select_idx);
        //计算
        calculator.doCalculate(cv, lck);

        if (calculator.select_idx == calculator.index)
        {
//            queryResponser.printSaleInfo();
//            queryResponser.randomSelect();
            //交互部分
//            cout << R"(calculate over, input 'q' or 'quit' to quit)" << endl;
//            cout
//                << R"(input 'random' do random query, 'select b/brand {a}{b}' to query brand {a}{b},)" << endl
//                << R"('select c/container {con}' to query container {con},')" << endl
//                << R"('select b/brand {a}{b} c/container {con}' or 'select c/container {con} b/brand {a}{b}' to query brand{a}{b} and container {con}.)"
//                << endl;
//            cout << "input:";
            // interactThread.join();
            cv.wait(lck);
        }
        else
        {
            cout << "calculate over. program exit" << endl;
            SYS_PAUSE;
        }
    }
}
