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
                                        if(calculator.reader.load_flag)
                                          continue;
                                        calculator.reader.load_flag=true;
                                        cv.notify_one();
                                        break;
                                    }
                                    case message::start:{
                                        //计算同步
                                        if(calculator.select_flag)
                                          continue;
                                        calculator.select_flag=true;
                                        auto param=Utils::unZipStart(m.getBody());
                                        int idx=get<0>(param);
                                        string con=get<1>(param);
                                        pair<int,int> brand=get<2>(param);
                                        calculator.select_idx=idx;
                                        calculator.targetBrand=brand;
                                        calculator.targetContainer=con;
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
                                        // if(calculator.is_notify)
                                        //     break;
                                            //没结束，忽略
                                        // else if(!calculator.is_finished)
                                        //     continue;
                                        
                                        string temp=m.getBody();
                                        int len = temp.length(), dif;
                                        for (int i = 0; i < len; i++)
                                            if (temp[i] == '|')
                                            {
                                                dif = i+1;
                                                break;
                                            }
                                        // cout<<"finish::"<<temp<<endl;
                                        int idx=stoi(temp.substr(0,dif));
                                        // cout<<"idx::"<<idx<<endl;
                                        temp=temp.substr(dif);
                                        // cout<<"cnt::"<<temp<<endl;
                                        // cout<<"idx:"<<idx<<" is_notify:"<<calculator.is_notify.count(idx)<<endl;
                                        // cout<<"idx:"<<idx<<" is_finished:"<<calculator.is_finished.count(idx)<<endl;

                                        //已被通知，忽略
                                        if(calculator.is_notify.count(idx))
                                            continue;
                                            //没结束，忽略
                                        if(!calculator.is_finished.count(idx))
                                            continue;
                                        if(calculator.finish_index!=idx)//finish标号与当前的同步标号不同，则直接舍弃
                                            continue;
                                        //检查是否满足同步条件
                                        if(calculator.checkFinish(temp.c_str()))
                                        {
                                            if(has_notify.count(idx))
                                              continue;
                                            //转发
                                            calculator.sendFinish(temp.c_str(),idx);
                                            //通知
                                            // calculator.setNotify(true);
                                            cv.notify_one();
                                            // cout<<"notify:"<<idx<<"!"<<endl;
                                            has_notify[idx]=1;
                                        }
                                        else if(calculator.is_finished.count(idx))//已经结束
                                        {
                                            //转发
                                            calculator.sendFinish(temp.c_str(),idx);
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
                                    case message::partKey:{
                                        auto partKey=(string)m.getBody();
                                        // cout<<"get partKey:"<<stoi(partKey)<<endl;
                                        int key=stoi(partKey);
                                        if(calculator.partKeyMap.count(key))
                                          continue;
                                        // calculator.partKeyMap[stoi(partKey)]=1;
                                        calculator.addPartKey(key);
                                        calculator.sendMessager->send(message(message::partKey, partKey.c_str()));
                                        break;
                                    }

                                    case message::finalPart: {

                                        auto mssg = Utils::unzipPartSaleData(m.getBody());
                                        if (mssg.second == this->port) continue;//自己发出去的转了一圈回来了，则不继续转发
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
