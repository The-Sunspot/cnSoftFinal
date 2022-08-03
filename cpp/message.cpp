//
// Created by 光球层上的黑子 on 2022/7/6.
//
#include <iostream>
#include "../head/message.h"

message::message(message::headEnum head, const char *body){
    //std::cout<<"create msg"<<body<<std::endl;
    memset(this->head,'\0',sizeof this->head);
    memset(this->body,'\0',sizeof this->body);
    memset(this->sendMsg,'\0',sizeof this->sendMsg);
    this->setHead(head);
    this->setBody(body);
}

message::message(message::headEnum head)    {
    memset(this->head,'\0',sizeof this->head);
    memset(this->body,'\0',sizeof this->body);
    memset(this->sendMsg,'\0',sizeof this->sendMsg);
    this->setHead(head);
}

message::message(const char *s){
    memset(this->head,'\0',sizeof this->head);
    memset(this->body,'\0',sizeof this->body);
    memset(this->sendMsg,'\0',sizeof this->sendMsg);
    for (int i = 0; i < MAX_HEAD_LENGTH; i++)
    {
        if (s[i] == ' ')
        {
            sendMsg[i] = ' ';
            head[i] = '\0';
        }
        else
            sendMsg[i] = head[i] = s[i];
    }
    for (int i = 0; i < MAX_BODY_LENGTH; i++)
    {
        sendMsg[i + MAX_HEAD_LENGTH] = body[i] = s[i + MAX_HEAD_LENGTH];
        if (s[i + MAX_HEAD_LENGTH] == '\0')
            break;
    }

};
const char *const message::getHead() const
{
    return head;
}
const char *const message::getBody() const
{
    return body;
}
const char *const message::getSendMsg() const
{
    return sendMsg;
}
int message::setBody(const char *s)
{
    strcpy(body, s);
    strcpy(sendMsg + MAX_HEAD_LENGTH, s);
    return 0;
}
void message::setHead(headEnum e)
{
    memset(head, 0, sizeof head);
    switch (e)
    {
        case load:
            strcpy(head, "l");
            break;
        case start:
            strcpy(head, "s");
            break;
        case order:
            strcpy(head, "o");
            break;
        case connect:
            strcpy(head, "c");
            break;
        case finish:
            strcpy(head, "f");
            break;
        case underSale:
            strcpy(head, "u");
            break;
        case totalData:
            strcpy(head, "t");
            break;
        case part:
            strcpy(head, "p");
            break;
        case partData:
            strcpy(head, "d");
            break;
        case unknown:
            strcpy(head, "x");
            break;
        case finalPart:
            strcpy(head, "P");
            break;
        case finalAns:
            strcpy(head, "A");
            break;
    }
    strcpy(sendMsg, head);
    for (int i = 0; i < MAX_HEAD_LENGTH; i++)
    {
        if (sendMsg[i] == '\0')
            sendMsg[i] = ' ';
    }
}
message::headEnum message::getHeadEnum(){
    if(strcmp(head,"l")==0) return load;
    else if(strcmp(head,"s")==0) return start;
    else if(strcmp(head,"o")==0) return order;
    else if(strcmp(head,"c")==0) return connect;
    else if(strcmp(head,"f")==0) return finish;
    else if(strcmp(head,"u")==0) return underSale;
    else if(strcmp(head,"t")==0) return totalData;
    else if(strcmp(head,"p")==0) return part;
    else if(strcmp(head,"d")==0) return partData;
    else if(strcmp(head,"P")==0) return finalPart;
    else if(strcmp(head,"A")==0) return finalAns;
    else return unknown;
}