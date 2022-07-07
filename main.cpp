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
    AdvancedCalculator calculator(totalData, reader);

    //接发信息类实例，
    server loadMessager(serverPort[index], cv, calculator, totalData);
    client *sendMessager = nullptr; // 1号实例顺序不同，暂不能赋值

    string s;
    // load
    if (index != 1)
    {
        cout << "wait for load. input \'load\' in 1st program to load datas." << endl;
        //建立连接
        sendMessager = new client(serverPort[index - 1], serverPort[index]);
        //等待同步
        cv.wait(lck);
        //告知
        sendMessager->sendLoad();
    }
    else
    { //主进程
        cout << "wait for load. input \'load\' to load datas." << endl;
        cout << "input: ";
        while (cin >> s)
        {
            if (s != "load")
            {
                cout << "please enter \"load\"!" << endl;
                cout << "input: ";
            }
            else
                break;
        }
        //建立连接
        sendMessager = new client(serverPort[n], serverPort[index]);
        //通知
        sendMessager->sendLoad();
        //同步
        cv.wait(lck);
    }
    //设置发送实例
    calculator.sendMessager = sendMessager;
    //读入数据
    calculator.loadDatas();

    // start
    //普通进程无需读入
    if (index != 1)
    {
        cout << "wait for calculate. input \'select\' in 1st program to do calculate and output." << endl;
        //等待同步
        cv.wait(lck);
        //通知
        sendMessager->sendStart();
        //计算
        calculator.doCalculate(cv, lck);
        cout << "calculate over. program exit" << endl;
        SYS_PAUSE;
        return 0;
    }
    else
    { //主进程
        cout << "wait for calculate. input \'select\' to do calculate and output." << endl;
        cout << "input: ";
        while (cin >> s)
        {
            if (s != "select")
            {
                cout << "please enter \"select\"!" << endl;
                cout << "input: ";
            }
            else
                break;
        }
        //传递
        sendMessager->sendStart();
        //同步
        cv.wait(lck);
        //计算
        calculator.doCalculate(cv, lck);

        //交互部分
        cout << R"(calculate over, input 'q' or 'quit' to quit)" << endl;
        cout
            << R"(input 'select Brand#{a}{b}' or 'select {a}{b}' or '{a}{b}' to show the Brand#{a}{b} details.(a and b must belong to 1~5))"
            << endl;
        cout << "input:";

        while (getline(cin, s))
        {
            //空行处理
            if (s.empty() || s == "\n" || s == "\r\n")
                continue;
            //退出
            if (s == "q" || s == "quit")
            {
                cout << "goodbye" << endl;
                return 0;
            }
            else
            {
                //根据Utils::judgeInput判断是否合法，并给予提示
                auto brand = Utils::judgeInput(s);
                if (brand == make_pair(-1, -1))
                {
                    cout << R"(input 'q' or 'quit' to quit)" << endl;
                    cout << R"(input 'select Brand#{a}{b}' or 'select {a}{b}' or '{a}{b}' to show the Brand#{a}{b} details.(a and b must belong to 1~5))" << endl;
                    cout << "input:";
                }
                else if (brand == make_pair(INT_MAX, INT_MAX))
                {
                    cout << "invalid brand code! (must belong to 1~5)" << endl;
                    cout << "input:";
                }
                else
                {
                    //合法，输出。
                    totalData.outputUnderSale(brand);
                    cout << "input:";
                }
            }
        }
    }
}
