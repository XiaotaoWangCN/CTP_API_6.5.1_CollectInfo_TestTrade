#include <iostream>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include<thread>
#include "CustomMdSpi.h"
#include "CustomTradeSpi.h"
//#include "TickToKlineHelper.h"
#include "BlockQueue.h"
#include "Timer.h"
#include <tinyxml2.h>
#define WriteFlag 0

using namespace std;
static BlockQueue<Timer> timer;
std::string day_week[]={"日","一","二","三","四","五","六"};
// 链接库
//#pragma comment (lib, "thostmduserapi.lib")
//#pragma comment (lib, "thosttraderapi.lib")

// ---- 全局变量 ---- //
// 公共参数
TThostFtdcBrokerIDType gBrokerID ;
TThostFtdcUserIDType gInvestorID ;
TThostFtdcPasswordType gInvestorPassword ;
TThostFtdcAppIDType gInvestorAppID ;
TThostFtdcAuthCodeType gInvestorAuthCode ;
char db_user_name[20];
char db_user_password[20];
char db_user_database[20];
char db_user_table[20];

// Market
CThostFtdcMdApi *g_pMdUserApi = nullptr;
CThostFtdcMdSpi *pMdUserSpi= nullptr;
char gMdFrontAddr[30] ;

// Trade
CThostFtdcTraderApi *g_pTradeUserApi = nullptr;
CustomTradeSpi *pTradeSpi = nullptr;
char gTradeFrontAddr[30] ;
TThostFtdcInstrumentIDType g_pTradeInstrumentID ;

void xmlread();
void timer_calc();
void timer_calc_init();
void market_start();
void trade_start();
void market_stop();
void trade_stop();
Order global_order;
double totalprofit=0;

int main()
{

    printf(" main.cpp global_order=%p\n",&global_order);
    std::thread t0 (xmlread);
    t0.join();
    std::thread t4 (timer_calc);
    std::thread t1 (CustomMdSpi::pre_write);
#if WriteFlag
    std::thread t2 (CustomMdSpi::write);
#endif
    std::thread t3 (CustomMdSpi::solve);
    //market_start();

    while(1)
    {
        Timer t=timer.take();
        if(t.wday>=1&&t.wday<=5)
        {
            if((t.hour==20&&t.min==55)||t.hour==8&&t.min==55)
            {
                market_start();
                trade_start();
            }
            if((t.hour==15&&t.min==20)||(t.hour==2&&t.min==35))
            {
                market_stop();
                trade_stop();
            }
            if((t.hour==9&&t.min==0)||t.hour==21&&t.min==0)
            {
                //std::cout<<"vector instruments size="<<CustomTradeSpi::instruments.size()<<std::endl;
                std::cout<<"Receive OnRspSubMarketData Instrument CNT="<<CustomMdSpi::instrument_cnt<<std::endl;
            }
        }
        if(t.wday==6)
        {
            if(t.hour==2&&t.min==35)
            {
                market_stop();
                trade_stop();
            }
        }

    }
}
void xmlread()
{
    tinyxml2::XMLDocument Doc;
    tinyxml2::XMLError xmlerror=Doc.LoadFile("./UserInfo.xml");
    if(xmlerror==tinyxml2::XML_SUCCESS)
    {
        tinyxml2::XMLElement *pRoot=Doc.RootElement();//获取根节点
        tinyxml2::XMLElement *pNode=pRoot->FirstChildElement("UserInfo");
        while (pNode)
        {
            tinyxml2::XMLElement *pChildNode=pNode->FirstChildElement();//获取第一个值为Value的子节点 默认返回第一个子节点
            const char* pContent;
            const tinyxml2::XMLAttribute *pAttributeOfNode = pNode->FirstAttribute();//获取第一个属性值
            //std::cout<< pAttributeOfNode->Value()<<":"<<std::endl;
            int res=0;
            while(pChildNode)
            {
                pContent=pChildNode->GetText();
                //std::cout<<pChildNode->FirstAttribute()->Value()<<":"<<pContent<<std::endl;
                pChildNode=pChildNode->NextSiblingElement();
                if(res==0)
                {
                    strcpy(gBrokerID,pContent);
                    std::cout<<gBrokerID<<std::endl;
                }
                else if (res==1)
                {
                    strcpy(gInvestorID,pContent);
                    std::cout<<gInvestorID<<std::endl;
                }
                else if (res==2)
                {
                    strcpy(gInvestorPassword,pContent);
                    std::cout<<gInvestorPassword<<std::endl;
                }
                else if (res==3)
                {
                    strcpy(gInvestorAppID,pContent);
                    std::cout<<gInvestorAppID<<std::endl;
                }
                else if (res==4)
                {
                    strcpy(gInvestorAuthCode,pContent);
                    std::cout<<gInvestorAuthCode<<std::endl;
                }
                else if (res==5)
                {
                    strcpy(gMdFrontAddr,pContent);
                    std::cout<<gMdFrontAddr<<std::endl;
                }
                else if (res==6)
                {
                    strcpy(gTradeFrontAddr,pContent);
                    std::cout<<gTradeFrontAddr<<std::endl;
                }
                else if (res==7)
                {
                    strcpy(g_pTradeInstrumentID,pContent);
                    std::cout<<g_pTradeInstrumentID<<std::endl;
                }
                else if (res==8)
                {
                    strcpy(db_user_name,pContent);
                    std::cout<<db_user_name<<std::endl;
                }
                else if (res==9)
                {
                    strcpy(db_user_password,pContent);
                    std::cout<<db_user_password<<std::endl;
                }
                else if (res==10)
                {
                    strcpy(db_user_database,pContent);
                    std::cout<<db_user_database<<std::endl;
                }
                else if (res==11)
                {
                    strcpy(db_user_table,pContent);
                    std::cout<<db_user_table<<std::endl;
                }
                res++;
            }
            //std::cout<<std::endl;
            pNode=pNode->NextSiblingElement();
        }
    }
    else
    {
        std::cout<<"XML ERROR="<<xmlerror<<endl;
    }




}
void timer_calc_init()
{
    Timer time_store;
    while(1)
    {
        time_t tt;
        time( &tt );
        tt = tt +8*3600;  // transform the time zone
        tm* t= gmtime( &tt );
        int client_year=t->tm_year+1900;
        int client_mon=t->tm_mon+1;
        int client_day=t->tm_mday;
        int client_wday=t->tm_wday;
        int client_hour= t->tm_hour;
        int client_min= t->tm_min;
        int client_sec=t->tm_sec;
        std::string test="";
        test=test+std::to_string(client_year)+"-";
        test=client_mon<10?test+"0"+std::to_string(client_mon)+"-":test+std::to_string(client_mon)+"-";
        test=client_day<10?test+"0"+std::to_string(client_day)+" ":test+std::to_string(client_day)+" ";
        test=test+"星期"+day_week[client_wday]+" ";
        test=client_hour<10?test+"0"+std::to_string(client_hour)+":":test+std::to_string(client_hour)+":";
        test=client_min<10?test+"0"+std::to_string(client_min)+":":test+std::to_string(client_min)+":";
        test=client_sec<10?test+"0"+std::to_string(client_sec):test+std::to_string(client_sec);
        std::cout<<test<<std::endl;
        time_store.year=client_year;
        time_store.mon=client_mon;
        time_store.day=client_day;
        time_store.wday=client_wday;
        time_store.hour=client_hour;
        time_store.min=client_min;
        timer.put(time_store);
        int left_min=5-client_min%5;
        int left_sec=left_min*60*1000;
        std::this_thread::sleep_for(std::chrono::milliseconds(left_sec));
    }
}
void timer_calc()
{
    timer_calc_init();
    Timer time_store;
    while(1)
    {
        time_t tt;
        time( &tt );
        tt = tt +8*3600;  // transform the time zone
        tm* t= gmtime( &tt );
        //std::cout<<t->tm_hour<<"\t"<<t->tm_min<<"\t"<<t->tm_sec<<std::endl;
        int client_year=t->tm_year+1900;
        int client_mon=t->tm_mon+1;
        int client_day=t->tm_mday;
        int client_wday=t->tm_wday;
        int client_hour= t->tm_hour;
        int client_min= t->tm_min;
        int client_sec=t->tm_sec;
        std::string test="";
        test=test+std::to_string(client_year)+"-";
        test=client_mon<10?test+"0"+std::to_string(client_mon)+"-":test+std::to_string(client_mon)+"-";
        test=client_day<10?test+"0"+std::to_string(client_day)+" ":test+std::to_string(client_day)+" ";
        test=test+"星期"+day_week[client_wday]+" ";
        test=client_hour<10?test+"0"+std::to_string(client_hour)+":":test+std::to_string(client_hour)+":";
        test=client_min<10?test+"0"+std::to_string(client_min)+":":test+std::to_string(client_min)+":";
        test=client_sec<10?test+"0"+std::to_string(client_sec):test+std::to_string(client_sec);
        //std::cout<<"Now Time "<<client_year<<"-"<<client_mon<<"-"<<client_day<<" "<<client_hour<<":"<<client_min<<":"<<client_sec<<std::endl;
        std::cout<<test<<std::endl;
        time_store.year=client_year;
        time_store.mon=client_mon;
        time_store.day=client_day;
        time_store.wday=client_wday;
        time_store.hour=client_hour;
        time_store.min=client_min;
        timer.put(time_store);
        //std::this_thread::sleep_for(std::chrono::milliseconds(6000000));//100分钟
        std::this_thread::sleep_for(std::chrono::milliseconds(300000));//5分钟
    }
}
void market_start()
{
    if(g_pMdUserApi== nullptr&&pMdUserSpi== nullptr)
    {
        cout << "market data..." << endl;
        CustomMdSpi::print_time();
        g_pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi();   // 创建行情实例
        pMdUserSpi = new CustomMdSpi;       // 创建行情回调实例
        g_pMdUserApi->RegisterSpi(pMdUserSpi);               // 注册事件类
        g_pMdUserApi->RegisterFront(gMdFrontAddr);           // 设置行情前置地址
        g_pMdUserApi->Init();                                // 连接运行
        std::cout<<"成功开启 Market Thread"<<std::endl;
    }
    else
    {
        std::cout<<"开启Market Thread 失败,Market Thread 指针不为空"<<std::endl;
    }

}
void trade_start()
{
    if(g_pTradeUserApi== nullptr&&pTradeSpi== nullptr)
    {
        cout << "Init Trade..." << endl;
        cout << "main.cpp g_pMdUserApi="<<g_pMdUserApi << endl;
        g_pTradeUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(); // 创建交易实例
        //CThostFtdcTraderSpi *pTradeSpi = new CustomTradeSpi;
        pTradeSpi = new CustomTradeSpi;               // 创建交易回调实例
        g_pTradeUserApi->RegisterSpi(pTradeSpi);                      // 注册事件类
        g_pTradeUserApi->SubscribePublicTopic(THOST_TERT_QUICK);    // 订阅公共流
        g_pTradeUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);   // 订阅私有流
        g_pTradeUserApi->RegisterFront(gTradeFrontAddr);              // 设置交易前置地址
        g_pTradeUserApi->Init();                                      // 连接运行
        std::cout<<"成功开启 Trade Thread"<<std::endl;
    }
    else
    {
        std::cout<<"开启Trade Thread 失败,Trade Thread 指针不为空"<<std::endl;
    }

}
void market_stop()
{
    std::cout<<"开始停止 Market Threading"<<std::endl;
    if(g_pMdUserApi!= nullptr&&pMdUserSpi!= nullptr)
    {
        g_pMdUserApi->RegisterSpi(nullptr);               // 注销事件类
        g_pMdUserApi->Release();
        g_pMdUserApi= nullptr;
        delete pMdUserSpi;
        pMdUserSpi= nullptr;
        std::cout<<"成功停止 Market Thread"<<std::endl;
        CustomMdSpi::instrument_cnt=0;
        //CustomMdSpi::requestID=0;

    }
    else
    {
        std::cout<<"Market Thread 指针为空，无法再次停止"<<std::endl;
    }
}

void trade_stop()
{
    std::cout<<"开始停止 Trade Threading"<<std::endl;
    if(g_pTradeUserApi!= nullptr&& pTradeSpi!= nullptr)
    {
        g_pTradeUserApi->RegisterSpi(nullptr);                      // 注销事件类
        g_pTradeUserApi->Release();
        g_pTradeUserApi= nullptr;
        delete pTradeSpi;
        pTradeSpi= nullptr;
        std::cout<<"成功停止 Trade Thread"<<std::endl;
        CustomTradeSpi::instruments.clear();
        //CustomTradeSpi::requestID=0;
    }
    else
    {
        std::cout<<"Trade Thread 指针为空，无法再次停止"<<std::endl;
    }
}
