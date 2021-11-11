#include <iostream>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include<thread>
#include "CustomMdSpi.h"
#include "CustomTradeSpi.h"
#include "BlockQueue.h"
#include "Timer.h"
using namespace std;
static BlockQueue<Timer> timer;
std::string day_week[]={"日","一","二","三","四","五","六"};
// 链接库
//#pragma comment (lib, "thostmduserapi.lib")
//#pragma comment (lib, "thosttraderapi.lib")


// 链接库
//#pragma comment (lib, "thostmduserapi.lib")
//#pragma comment (lib, "thosttraderapi.lib")

// ---- 全局变量 ---- //
// 公共参数
TThostFtdcBrokerIDType gBrokerID = "";//此处填写经纪商ID
TThostFtdcUserIDType gInvesterID = "";//此处填写用户ID
TThostFtdcPasswordType gInvesterPassword = "";//此处填写用户密码
TThostFtdcAppIDType gInvesterAppID = "";//用户申请的APPID;
TThostFtdcAuthCodeType gInvesterAuthCode = "";//用户申请的授权码;

// Market
CThostFtdcMdApi *g_pMdUserApi = nullptr;
CThostFtdcMdSpi *pMdUserSpi= nullptr;
char gMdFrontAddr[] = "tcp://114.80.54.99:42213";

// Trade
CThostFtdcTraderApi *g_pTradeUserApi = nullptr;
CustomTradeSpi *pTradeSpi = nullptr;
char gTradeFrontAddr[] = "tcp://114.80.54.99:42205";
TThostFtdcInstrumentIDType g_pTradeInstrumentID = "a2201";
TThostFtdcDirectionType gTradeDirection = THOST_FTDC_D_Sell;
TThostFtdcPriceType gLimitPrice = 65536;

void timer_calc();
void timer_calc_init();
void market_start();
void trade_start();
void market_stop();
void trade_stop();

void backup();
void remove();

int main()
{

    std::thread t4 (timer_calc);
    std::thread t1 (CustomMdSpi::pre_write);
    std::thread t2 (CustomMdSpi::write);
    while(1)
    {
        Timer t=timer.take();
        if(t.wday>=1&&t.wday<=5)
        {
            if((t.hour==8&&t.min==55)||t.hour==20&&t.min==55)
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
	     if(t.hour==15&&t.min==30)
            {
                    backup();
            }
            if(t.hour==16&&t.min==0)
            {
                    remove();
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
   
    getchar();
    return 0;
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
        g_pTradeUserApi->SubscribePublicTopic(THOST_TERT_RESTART);    // 订阅公共流
        g_pTradeUserApi->SubscribePrivateTopic(THOST_TERT_RESTART);   // 订阅私有流
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
    }
    else
    {
        std::cout<<"Trade Thread 指针为空，无法再次停止"<<std::endl;
    }
}

void backup()
{
    time_t tt;
    time( &tt );
    tt = tt +8*3600;  // transform the time zone
    tm* t= gmtime( &tt );
    int client_year=t->tm_year+1900;
    int client_mon=t->tm_mon+1;
    int client_day=t->tm_mday;
    std::string filename_pre="tick-";
    filename_pre=filename_pre+std::to_string(client_year)+"-";
    filename_pre=client_mon<10?filename_pre+"0"+std::to_string(client_mon)+"-":filename_pre+std::to_string(client_mon)+"-";
    filename_pre=client_day<10?filename_pre+"0"+std::to_string(client_day):filename_pre+std::to_string(client_day);
    string filename=filename_pre+".sql";
    std::string command_sqldump="mysqldump -uroot -proot test tick >~/Tick/"+filename;
    system(command_sqldump.c_str());
    //std::cout<<command_sqldump<<std::endl;
    string command_tar_save="tar -C ~/Tick -czvf ~/Tick/"+filename_pre+".tar.gz"+" "+filename;
    system(command_tar_save.c_str());
    std::cout<<command_tar_save<<std::endl;
    return;
}
void remove()
{
    MYSQL conn;
    int res;
    MYSQL *stdcall;
    mysql_init(&conn);
    stdcall=mysql_real_connect(&conn, "localhost","root","root","test",0,NULL,CLIENT_FOUND_ROWS);
    if(stdcall)
    {
        std::cout << "connect success" << std::endl;
        time_t tt;
        time( &tt );
        tt = tt +8*3600;  // transform the time zone
        tm* t= gmtime( &tt );
        int client_year=t->tm_year+1900;
        int client_mon=t->tm_mon+1;
        int client_day=t->tm_mday;
        std::string filename_pre="tick-";
        filename_pre=filename_pre+std::to_string(client_year)+"-";
        filename_pre=client_mon<10?filename_pre+"0"+std::to_string(client_mon)+"-":filename_pre+std::to_string(client_mon)+"-";
        filename_pre=client_day<10?filename_pre+"0"+std::to_string(client_day):filename_pre+std::to_string(client_day);
        string filename=filename_pre+".sql";
        string command_rm_rf="rm -rf ~/Tick/"+filename;
        system(command_rm_rf.c_str());
        std::cout<<command_rm_rf<<std::endl;
        std::string querystr="select count(*) from  tick";
        res = mysql_query(&conn, querystr.c_str());
        if(res)
        {
            std::cout<<"res="<<res<<std::endl;
            std::cout<<mysql_error(&conn)<<std::endl;
        }
        else
        {
            MYSQL_RES *result;
            result = mysql_store_result(&conn);
            MYSQL_ROW row;
            row = mysql_fetch_row(result);
            std::cout<<"select count(*) from tick =[ "<<row[0]<<" ]"<<std::endl;
            querystr="drop table tick";
            res = mysql_query(&conn, querystr.c_str());
            if(res)
            {
                std::cout<<"res="<<res<<std::endl;
                std::cout<<mysql_error(&conn)<<std::endl;
            }
            else
            {
                std::cout<<querystr<<std::endl;
                querystr="CREATE TABLE `tick` (\n"
                         "  `TradingDay` char(8) DEFAULT NULL,\n"
                         "  `ExchangeID` char(5) DEFAULT NULL,\n"
                         "  `LastPrice` decimal(9,3) DEFAULT NULL,\n"
                         "  `Volume` int DEFAULT NULL,\n"
                         "  `OpenInterest` int DEFAULT NULL,\n"
                         "  `UpdateTime` char(8) DEFAULT NULL,\n"
                         "  `UpdateMillisec` int DEFAULT NULL,\n"
                         "  `BidPrice1` decimal(9,3) DEFAULT NULL,\n"
                         "  `BidVolume1` int DEFAULT NULL,\n"
                         "  `AskPrice1` decimal(9,3) DEFAULT NULL,\n"
                         "  `AskVolume1` int DEFAULT NULL,\n"
                         "  `InstrumentID` char(18) DEFAULT NULL\n"
                         ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;";
                res = mysql_query(&conn, querystr.c_str());
                std::cout<<"Create Table Finished."<<std::endl;
                if(res)
                {
                    std::cout<<"res="<<res<<std::endl;
                    std::cout<<mysql_error(&conn)<<std::endl;
                }
                else
                {
                    querystr="reset master";
                    res = mysql_query(&conn, querystr.c_str());
                    if(res)
                    {
                        std::cout<<"res="<<res<<std::endl;
                        std::cout<<mysql_error(&conn)<<std::endl;
                    }
                    else
                    {
                        std::cout<<querystr<<std::endl;
                        mysql_close(&conn);
                    }
                }
            }
        }
    }
    else
    {
        std::cout<<mysql_error(&conn)<<std::endl;
    }
    return ;
}
