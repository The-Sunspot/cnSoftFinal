//
// Created by 光球层上的黑子 on 2022/7/1.
//

#include "../head/server.h"

server::server(int p, condition_variable &cv, Calculator &calculator, TotalData &data) : port(p),
                                                                                         acceptor(ios, ip::tcp::endpoint(ip::tcp::v4(), port)),
                                                                                         sock(ios),
                                                                                         totalData(data)
{
    std::thread serverListenThread([&]
                                   {
                        //监听
                        acceptor.listen();
                        acceptor.accept(sock);
                        //阻塞等待
                        while (true)
                        {
                            try
                            {
                                //阻塞读
                                read(sock, buffer(msgFromClient));
                                message m(msgFromClient);
                                switch (m.getHeadEnum()) {
                                    case message::connect:{
                                        break;
                                    }
                                    case message::load:{
                                        //读取同步
                                        calculator.reader.load_flag=true;
                                        cv.notify_one();
                                        break;
                                    }
                                    case message::start:{
                                        //计算同步
                                        calculator.select_flag=true;
                                        int idx=m.getBody()[0]-'0';
                                        // cout<<"select_idx:"<<idx<<endl;
                                        calculator.select_idx=idx;
                                        cv.notify_one();
                                        break;
                                    }
                                    case message::order:{
                                        //解析，计算
                                        auto temp=Utils::unZipOrderString(m.getBody());
                                        calculator.solveOrder(temp);
                                        break;
                                    }
                                    case message::finish:{
                                        //已被通知，忽略
                                        if(calculator.is_notify)
                                            break;
                                            //没结束，忽略
                                        else if(!calculator.is_finished)
                                            continue;
                                        //检查是否满足同步条件
                                        if(calculator.checkFinish(m.getBody()))
                                        {
                                            //转发
                                            calculator.sendFinish(m.getBody());
                                            //通知
                                            // calculator.setNotify(true);
                                            cv.notify_one();
                                        }
                                            //已经结束
                                        else if(calculator.is_finished)
                                        {
                                            //转发
                                            calculator.sendFinish(m.getBody());
                                        }
                                        break;
                                    }
                                    case message::underSale:{
                                        auto part= Utils::unzipPartData(m.getBody());
                                        //尝试插入
                                        bool insertFlag=calculator.tryInsertUndersalePart(part);
                                        //插入成功，说明不是自己发的
                                        if(insertFlag){
                                            // 转发
                                            calculator.transferUndersalePartData(m);
                                        }
                                        break;
                                    }
                                    case message::totalData:{
                                        //解析
                                        auto d=Utils::unZipTotalData(m.getBody());
                                        if(d.first==port){
                                            //本机发送的请求
                                            continue;
                                        }
                                        //合并
                                        totalData.addPartCount(d.second.first);
                                        totalData.addTotalSales(d.second.second);
                                        //转发
                                        calculator.transferTotalData(m);
                                        break;
                                    }
                                    case message::part:{
                                        //解析，并插入
                                        auto part_=Utils::unZipPartString(m.getBody());
                                        //能插入说明不是本机发送的，继续转发
                                        if(calculator.reader.insertPart(part_)){
                                            calculator.sendPart(part_);
                                        }
                                        break;
                                    }
                                    case message::partData:{
                                        //合并partData
                                        auto partData= Utils::unzipPartData(m.getBody());
                                        calculator.totalData.mergePartData(partData);
                                        break;
                                    }

                                    case message::finalPart: {

                                        auto mssg = Utils::unzipPartSaleData(m.getBody());
                                        if (mssg.second == this->port) continue;
                                        calculator.data.insertReceivePartData(mssg.first);
                                        //calculator.data.insertPartData(mssg.first);
                                        calculator.sendMessager->send(m);
                                    }
                                    case message::finalAns: {
                                        long double ans = stold(m.getBody());
                                        calculator.data.addUnderSales(ans);
                                        //calculator.data.underSales += ans;
                                        break;
                                    }

                                    case message::unknown:
                                        break;
                                }
                            }
                            //这里是一些关于socket结束的信息
                            //比如读到EOF等等，可以不用理会
                            //直接结束即可。
                            catch (std::system_error e)
                            {
                                //cerr<<"read error"<<endl;
                                //cerr << e.what() << endl;
                                sock.close();
                                ios.stop();
                                break;
                            }
                        } });
    //启动服务
    serverListenThread.detach();
    cout << "server init over. connectServerPort:" << port << endl;
};
