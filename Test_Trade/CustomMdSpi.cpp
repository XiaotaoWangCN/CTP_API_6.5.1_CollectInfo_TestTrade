#include <iostream>
#include <fstream>
#include <unordered_map>
#include "CustomMdSpi.h"
#include<cstring>
#include <iomanip>
#define Profit_Percent_Rate 1.9

#define WriteFlag 0
#define ROLLBACK 0
#define CONTINUE 1
#define STOP 0
#define DETAILINFO 1
// 互斥量与条件变量

// ---- 全局参数声明 ---- //
extern CThostFtdcMdApi *g_pMdUserApi;
extern char gMdFrontAddr[];
extern TThostFtdcBrokerIDType gBrokerID;
extern TThostFtdcInvestorIDType gInvestorID;
extern TThostFtdcPasswordType gInvestorPassword;

extern TThostFtdcInstrumentIDType g_pTradeInstrumentID;       // 合约代码
extern CustomTradeSpi *pTradeSpi ;

extern char db_user_name[20];
extern char db_user_password[20];
extern char db_user_database[20];
extern char db_user_table[20];
extern Order global_order;
extern double totalprofit;

BlockQueue<BarData > CustomMdSpi::marketDataQueue;
BlockQueue<BarData > CustomMdSpi::saveDataQueue;
BarData CustomMdSpi::bararray[SIZE];



std::vector<Kindle> CustomMdSpi::kindle,CustomMdSpi::kindle_hour;
std::vector<Tick> CustomMdSpi::tick;
std::vector<double> CustomMdSpi::ema_fast_period,CustomMdSpi::ema_slow_period;
std::vector<int> CustomMdSpi::volume;
std::vector<double> CustomMdSpi::ma,CustomMdSpi::ma_80,CustomMdSpi::ma_360;
std::vector<double>CustomMdSpi::diff,CustomMdSpi::dea,CustomMdSpi::macd;
std::vector <double> CustomMdSpi::close_price_array;
std::vector<std::vector<std::vector<int> > > CustomMdSpi::data=std::vector<std::vector<std::vector<int> > >(24);//创建2个vector<vector<int> >类型的数组
std::vector<double> CustomMdSpi::rsi_fast_period_up,CustomMdSpi::rsi_mid_period_up,CustomMdSpi::rsi_slow_period_up;
std::vector<double> CustomMdSpi::rsi_fast_period_down,CustomMdSpi::rsi_mid_period_down,CustomMdSpi::rsi_slow_period_down;
std::vector<double> CustomMdSpi::rsi_fast_period_smma_up,CustomMdSpi::rsi_mid_period_smma_up,CustomMdSpi::rsi_slow_period_smma_up;
std::vector<double> CustomMdSpi::rsi_fast_period_smma_down,CustomMdSpi::rsi_mid_period_smma_down,CustomMdSpi::rsi_slow_period_smma_down;
std::vector<double> CustomMdSpi::rsi_fast_period_rs,CustomMdSpi::rsi_mid_period_rs,CustomMdSpi::rsi_slow_period_rs;
std::vector<double> CustomMdSpi::rsi_fast_period_rsi,CustomMdSpi::rsi_mid_period_rsi,CustomMdSpi::rsi_slow_period_rsi;
Kindle CustomMdSpi::test_kindle,CustomMdSpi::test_kindle_hour;
int CustomMdSpi::cnt=0;
int CustomMdSpi::requestID=0;

int CustomMdSpi::instrument_cnt=0;
double * query_array;
double * query_array2;
int totalyingli=0;
int kaiduoyingli=0;
int kaikongyingli=0;
int totalkuisun=0;
int kaiduokuisun=0;
int kaikongkuisun=0;
int shoushuyingli=0;
int kaiduoshoushuyingli=0;
int kaikongshoushuyingli=0;
int shoushukuisun=0;
int kaiduoshoushukuisun=0;
int kaikongshoushukuisun=0;
double* Bill(std::vector<Kindle> kindle,double LastPrice,double *A);
// ---- ctp_api回调函数 ---- //
// 连接成功应答
void CustomMdSpi::OnFrontConnected()
{
	// 开始登录
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq));
	strcpy(loginReq.BrokerID, gBrokerID);
	strcpy(loginReq.UserID, gInvestorID);
	strcpy(loginReq.Password, gInvestorPassword);
	int rt = g_pMdUserApi->ReqUserLogin(&loginReq, ++requestID);
	if(query_array== nullptr)
	{
	    query_array=new double [6];
	}
	if(query_array2== nullptr)
	{
	    query_array2=new double [6];
	}

}

// 断开连接通知
void CustomMdSpi::OnFrontDisconnected(int nReason)
{
	std::cerr << "=====网络连接断开=====" << std::endl;
	print_time();
	std::cerr << "错误码： " << nReason << std::endl;
}

// 心跳超时警告
void CustomMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cerr << "=====网络心跳超时=====" << std::endl;
    print_time();
	std::cerr << "距上次连接时间： " << nTimeLapse << std::endl;
}

// 登录应答
void CustomMdSpi::OnRspUserLogin(
	CThostFtdcRspUserLoginField *pRspUserLogin, 
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
        std::cout << "CustoMdSpi.cpp g_pMdUserApi="<<g_pMdUserApi << std::endl;
		// 开始订阅行情
		/*
        std::ifstream filein;
        filein.open("../pair.csv");
        std::string test[658];
        std::string temp;
        int res=0;
        while(getline(filein,temp,','))
        {
            std::string key,value;
            filein>>test[res];
            char c[2];
            filein.getline(c,2);
            g_pInstrumentID[res]= (char*)(test[res].c_str());
            res++;
        }
        filein.close();
		int rt = g_pMdUserApi->SubscribeMarketData(g_pInstrumentID, instrumentNum);
*/


	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 登出应答
void CustomMdSpi::OnRspUserLogout(
	CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout <<  std::endl;
		std::cout << pUserLogout->BrokerID << std::endl;
		std::cout << pUserLogout->UserID << std::endl;
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 错误通知
void CustomMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
    if (bResult)
        std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 订阅行情应答
void CustomMdSpi::OnRspSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
        instrument_cnt++;
		;
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 取消订阅行情应答
void CustomMdSpi::OnRspUnSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
	    /*
	    if(bIsLast==true)
        {
            std::cout<<"成功退订所有行情"<<std::endl;
        }
        */
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 订阅询价应答
void CustomMdSpi::OnRspSubForQuoteRsp(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 取消订阅询价应答
void CustomMdSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 行情详情通知
void CustomMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
    //std::cout<<"Call Method"<<std::endl;

  	// 打印行情，字段较多，截取部分
    BarData bardata;
    bardata.TradingDay=pDepthMarketData->TradingDay;
    bardata.ExchangeID=pDepthMarketData->ExchangeID;
    bardata.LastPrice=pDepthMarketData->LastPrice;
    bardata.Volume=pDepthMarketData->Volume;
    bardata.OpenInterest=pDepthMarketData->OpenInterest;
    bardata.UpdateTime=pDepthMarketData->UpdateTime;
    bardata.UpdateMillisec=pDepthMarketData->UpdateMillisec;
    bardata.BidPrice1=pDepthMarketData->BidPrice1;
    bardata.BidVolume1=pDepthMarketData->BidVolume1;
    bardata.AskPrice1=pDepthMarketData->AskPrice1;
    bardata.AskVolume1=pDepthMarketData->AskVolume1;
    bardata.InstrumentID=pDepthMarketData->InstrumentID;
    marketDataQueue.put(bardata);
    //std::cout <<"bar:["<<  bardata.TradingDay <<","<< bardata.InstrumentID<<","<< bardata.LastPrice<<","<<bardata.UpdateTime<<","<<bardata.Volume<<"]"<<std::endl;



	// 取消订阅行情
	//int rt = g_pMdUserApi->UnSubscribeMarketData(g_pInstrumentID, instrumentNum);
	//if (!rt)
	//	std::cout << ">>>>>>发送取消订阅行情请求成功" << std::endl;
	//else
	//	std::cerr << "--->>>发送取消订阅行情请求失败" << std::endl;
}

// 询价详情通知
void CustomMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	// 部分询价结果
	std::cout << "=====获得询价结果=====" << std::endl;
	std::cout <<  pForQuoteRsp->TradingDay << std::endl;
	std::cout <<  pForQuoteRsp->InstrumentID << std::endl;
	std::cout << pForQuoteRsp->ForQuoteSysID << std::endl;
}
void CustomMdSpi::print_time()
{
    time_t tt;
    time( &tt );
    tt = tt +8*3600;  // transform the time zone
    tm* t= gmtime( &tt );
    //std::cout<<t->tm_hour<<"\t"<<t->tm_min<<"\t"<<t->tm_sec<<std::endl;
    int client_year=t->tm_year+1900;
    int client_mon=t->tm_mon+1;
    int client_day=t->tm_mday;
    int client_hour= t->tm_hour;;
    int client_min= t->tm_min;
    int client_sec=t->tm_sec;
    std::string test="";
    test=test+std::to_string(client_year)+"-";
    test=client_mon<10?test+"0"+std::to_string(client_mon)+"-":test+std::to_string(client_mon)+"-";
    test=client_day<10?test+"0"+std::to_string(client_day)+" ":test+std::to_string(client_day)+" ";
    test=client_hour<10?test+"0"+std::to_string(client_hour)+":":test+std::to_string(client_hour)+":";
    test=client_min<10?test+"0"+std::to_string(client_min)+":":test+std::to_string(client_min)+":";
    test=client_sec<10?test+"0"+std::to_string(client_sec):test+std::to_string(client_sec);
    //std::cout<<"Now Time "<<client_year<<"-"<<client_mon<<"-"<<client_day<<" "<<client_hour<<":"<<client_min<<":"<<client_sec<<std::endl;
    std::cout<<test<<std::endl;
}
void CustomMdSpi::print_mysql_error_time()
{
    time_t tt;
    time( &tt );
    tt = tt +8*3600;  // transform the time zone
    tm* t= gmtime( &tt );
    //std::cout<<t->tm_hour<<"\t"<<t->tm_min<<"\t"<<t->tm_sec<<std::endl;
    int client_year=t->tm_year+1900;
    int client_mon=t->tm_mon+1;
    int client_day=t->tm_mday;
    int client_hour= t->tm_hour;;
    int client_min= t->tm_min;
    int client_sec=t->tm_sec;
    int client_totalsec=client_hour*3600+client_min*60+client_sec;
    std::cout<<"It is "<<client_year<<"-"<<client_mon<<"-"<<client_day<<" "<<client_hour<<":"<<client_min<<":"<<client_sec<<" MySQL Lost  Connection"<<std::endl;
    print_time();
}
void CustomMdSpi::pre_write()
{
    std::cout<<"pre write"<<std::endl;
    print_time();
    int inf=1<<30;
    while(1)
    {
        BarData bardata =marketDataQueue.take();
        int server_hour= std::stoi(bardata.UpdateTime.substr(0,2));;
        int server_minute= std::stoi(bardata.UpdateTime.substr(3,2));
        int server_second=std::stoi(bardata.UpdateTime.substr(6,2));
        int server_totalsec=server_hour*3600+server_minute*60+server_second;
        int BidVolume1=bardata.BidVolume1;
        int AskVolume1=bardata.AskVolume1;
        std::string ExchangeID= bardata.ExchangeID;
        bool flag=true;
        if((BidVolume1>=0&& AskVolume1>0)||(BidVolume1>0&& AskVolume1>=0))
        {
            if(bardata.LastPrice>inf)
            {
                //bardata.LastPrice=0;
                flag= false;

            }
            if(bardata.OpenInterest>inf)
            {
                //bardata.OpenInterest=0;
                flag=false;
            }
            if(bardata.BidPrice1>inf)
            {
                bardata.BidPrice1=0;
                //flag=false;
            }
            if(bardata.AskPrice1>inf)
            {
                bardata.AskPrice1=0;
                //flag=false;
            }
            if(flag==true)
            {
                //if(ExchangeID.compare("SHFE")==0||ExchangeID.compare("CZCE")==0||ExchangeID.compare("INE")==0||ExchangeID.compare("DCE")==0)
                {
                    //if((32100<=server_totalsec&&server_totalsec<=36900)||(37800<=server_totalsec&&server_totalsec<=41400)||(48600<=server_totalsec&&server_totalsec<=54000)||(75300<=server_totalsec&&server_totalsec<=82800)||(82800<=server_totalsec&&server_totalsec<=86400)||(0<=server_totalsec&&server_totalsec<=9000))
                    {
                        saveDataQueue.put(bardata);

                    }
                }
                //else if (ExchangeID.compare("CFFEX")==0)
                {
                    //if((32100<=server_totalsec&&server_totalsec<=41400)||(46800<=server_totalsec&&server_totalsec<=54900)||(75300<=server_totalsec&&server_totalsec<=82800)||(82800<=server_totalsec&&server_totalsec<=86400)||(0<=server_totalsec&&server_totalsec<=9000))
                    {
                       // saveDataQueue.put(bardata);
                    }
                }
            }

        }
    }
}
#if WriteFlag
void CustomMdSpi::write()
{
    std::cout<<"begin run write"<<std::endl;
    print_time();
    MYSQL conn;
    int res;
    mysql_init(&conn);
    int cnt=0;
    //"root":数据库管理员 "root":root密码 "test":数据库的名字
    MYSQL * connect_state=mysql_real_connect(&conn, "localhost","root","root","test",0,NULL,CLIENT_FOUND_ROWS);
    //std::cout<<connect_state<<std::endl;
    if (connect_state)
    {
        while(1)
        {
            BarData bardata =saveDataQueue.take();
            int server_hour= std::stoi(bardata.UpdateTime.substr(0,2));;
            int server_minute= std::stoi(bardata.UpdateTime.substr(3,2));
            int server_second=std::stoi(bardata.UpdateTime.substr(6,2));
            int server_totalsec=server_hour*3600+server_minute*60+server_second;

            bararray[cnt++]=bardata;
            if(cnt==SIZE)
            {
                std::string  test="insert into tick values";
                for(int i=0;i<SIZE;++i)
                {
                    if(i!=SIZE-1)
                    {
                        std::string temp="('"+bararray[i].TradingDay+"','"+bararray[i].ExchangeID+"','"+std::to_string(bararray[i].LastPrice)+"','"+std::to_string(bararray[i].Volume)
                                         +"','"+std::to_string(bararray[i].OpenInterest)+"','"+bararray[i].UpdateTime+"','"+std::to_string(bararray[i].UpdateMillisec)+"','"+std::to_string(bararray[i].BidPrice1)
                                         +"','"+std::to_string(bararray[i].BidVolume1)+"','"+std::to_string(bararray[i].AskPrice1)+"','"+std::to_string(bararray[i].AskVolume1)+"','"+bararray[i].InstrumentID+"'),";
                        test+=temp;
                    }
                    else
                    {
                        std::string temp="('"+bararray[i].TradingDay+"','"+bararray[i].ExchangeID+"','"+std::to_string(bararray[i].LastPrice)+"','"+std::to_string(bararray[i].Volume)
                                         +"','"+std::to_string(bararray[i].OpenInterest)+"','"+bararray[i].UpdateTime+"','"+std::to_string(bararray[i].UpdateMillisec)+"','"+std::to_string(bararray[i].BidPrice1)
                                         +"','"+std::to_string(bararray[i].BidVolume1)+"','"+std::to_string(bararray[i].AskPrice1)+"','"+std::to_string(bararray[i].AskVolume1)+"','"+bararray[i].InstrumentID+"')";
                        test+=temp;
                    }
                }
                res = mysql_query(&conn, test.c_str());
                if(res)
                {
                    print_mysql_error_time();
                    std::cout <<test << std::endl;
                    std::cout<<"ERROR-res\t"<<mysql_error(&conn)<<std::endl;
                    mysql_close(&conn);
                    connect_state=mysql_real_connect(&conn, "localhost","root","root","test",0,NULL,CLIENT_FOUND_ROWS);
                    res = mysql_query(&conn, test.c_str());
                }
                else
                {
                    //std::cout <<test << std::endl;
                    //print_time();
                    //std::cout <<test << std::endl;
                    //printf("OK\n");
                }
                cnt=0;
            }
            else if((server_totalsec>=36890&&server_totalsec<=36900)||(server_totalsec>=41390&&server_totalsec<=41400)||(server_totalsec>=53990&&server_totalsec<=54000)||(server_totalsec>=54890&&server_totalsec<=54900)||(server_totalsec>=82790&&server_totalsec<=82800)||(server_totalsec>=8990&&server_totalsec<=9000))
            {
                std::string  test="insert into tick values";
                for(int i=0;i<cnt;++i)
                {
                    if(i!=cnt-1)
                    {
                        std::string temp="('"+bararray[i].TradingDay+"','"+bararray[i].ExchangeID+"','"+std::to_string(bararray[i].LastPrice)+"','"+std::to_string(bararray[i].Volume)
                                         +"','"+std::to_string(bararray[i].OpenInterest)+"','"+bararray[i].UpdateTime+"','"+std::to_string(bararray[i].UpdateMillisec)+"','"+std::to_string(bararray[i].BidPrice1)
                                         +"','"+std::to_string(bararray[i].BidVolume1)+"','"+std::to_string(bararray[i].AskPrice1)+"','"+std::to_string(bararray[i].AskVolume1)+"','"+bararray[i].InstrumentID+"'),";
                        test+=temp;
                    }
                    else
                    {
                        std::string temp="('"+bararray[i].TradingDay+"','"+bararray[i].ExchangeID+"','"+std::to_string(bararray[i].LastPrice)+"','"+std::to_string(bararray[i].Volume)
                                         +"','"+std::to_string(bararray[i].OpenInterest)+"','"+bararray[i].UpdateTime+"','"+std::to_string(bararray[i].UpdateMillisec)+"','"+std::to_string(bararray[i].BidPrice1)
                                         +"','"+std::to_string(bararray[i].BidVolume1)+"','"+std::to_string(bararray[i].AskPrice1)+"','"+std::to_string(bararray[i].AskVolume1)+"','"+bararray[i].InstrumentID+"')";
                        test+=temp;
                    }
                }
                res = mysql_query(&conn, test.c_str());
                if(res)
                {
                    print_mysql_error_time();
                    std::cout <<test << std::endl;
                    std::cout<<"ERROR-res\t"<<mysql_error(&conn)<<std::endl;
                    mysql_close(&conn);
                    connect_state=mysql_real_connect(&conn, "localhost","root","root","test",0,NULL,CLIENT_FOUND_ROWS);
                    res = mysql_query(&conn, test.c_str());
                }
                else
                {
                    //std::cout <<test << std::endl;
                    //print_time();
                    //std::cout <<test << std::endl;
                    //printf("OK\n");
                }
                cnt=0;
            }
            //mysql_close(&conn);
            //std::cout <<"bar:["<<  bardata.TradingDay <<","<< bardata.InstrumentID<<","<< bardata.LastPrice<<","<<bardata.UpdateTime<<","<<bardata.Volume<<"]"<<std::endl;
        }
    }
    else
    {
        std::cout<<"ERROR\t"<<mysql_error(&conn)<<std::endl;
        print_time();
        mysql_close(&conn);
        std::cout << "connect failed" << std::endl;
    }
}
#endif

void CustomMdSpi::pre_solve()
{

    int totalyingli=0;
    int kaiduoyingli=0;
    int kaikongyingli=0;
    int totalkuisun=0;
    int kaiduokuisun=0;
    int kaikongkuisun=0;
    int shoushuyingli=0;
    int kaiduoshoushuyingli=0;
    int kaikongshoushuyingli=0;
    int shoushukuisun=0;
    int kaiduoshoushukuisun=0;
    int kaikongshoushukuisun=0;

    MYSQL conn;
    int res;
    int data_column;
    int data_row;
    MYSQL_RES *result;
    MYSQL_ROW row;
    MYSQL *stdcall;
    mysql_init(&conn);
    printf(" CustoMdspi.cpp global_order=%p\n",&global_order);
    if(query_array2== nullptr)
    {
        query_array2=new double [6];
    }

    for (int n = 0; n < 24; n++)
    {
        data[n].resize(60);
    }//要先对二维数组设定大小
    clock_t start,end;

    stdcall=mysql_real_connect(&conn, "localhost",db_user_name,db_user_password,db_user_database,0,NULL,CLIENT_FOUND_ROWS);
    //Order order;
    //global_order.open_price=0;
    //global_order.direction=0;
    double total=0;
    if(stdcall)
    {
        std::cout << "connect success" << std::endl;
        //std::string querystr="select * from  tick where InstrumentID='AP201' ";
        std::string querystr="select * from "+std::string(db_user_table)+" ";
        //select * from data.tick where TradingDay='20210811' or TradingDay='20210822';

        res = mysql_query(&conn, querystr.c_str());
        if(res)
        {
            std::cout<<"res="<<res<<std::endl;
            std::cout<<mysql_error(&conn)<<std::endl;
        }
        else
        {
            result = mysql_store_result(&conn);
            data_column = mysql_num_fields(result);
            std::cout<<"datacolumn="<<data_column<<std::endl;
            data_row=mysql_num_rows(result);
            std::cout<<"datarow="<<data_row<<std::endl;
            int row_cnt=0;
            int last_min=-1;
            int last_hour=-1;
            int last_volume=0;
            int current_volume=0;
            int last_min_total_volume=0;
            double topprofit=0;
            double totalprofit=0;
            start=clock();

            double open_hour=0;
            double close_hour=0;
            double low_hour=1000000;
            double high_hour=-1;
            if(query_array== nullptr)
            {
                query_array=new double [6];
            }
            global_order.open_price=0;
            global_order.direction=0;
            global_order.loss_price=0;
            global_order.segment=0;
            global_order.open_flag= false;
            global_order.TradingDay="0000";
            global_order.volume=0;
            global_order.stop_profit=0;
            //global_order.last_second_price=0;
            int test1,test2,test3,test4;
            while ((row = mysql_fetch_row(result)))  //遇到最后一行，则中止循环
                {
                int Volume=atoi(row[3]);
                int OpenInterest=atoi(row[4]);
                int BidVolume1=atoi(row[8]);
                int AskVolume1=atoi(row[10]);
                std::string UpdateTime=row[5];

                Tick test_tick;
                test_tick.TradingDay=row[0];
                test_tick.LastPrice=atof(row[2]);
                test_tick.Volume=atoi(row[3]);
                test_tick.OpenInterest=atoi(row[4]);
                test_tick.UpdateTime=row[5];
                test_tick.UpdateMillisec=atoi(row[6]);
                test_tick.InstrumentID=row[11];
                test_tick.Volume= atoi(row[3]);
                tick.push_back(test_tick);
                int current_hour= std::stoi(tick[cnt].UpdateTime.substr(0,2));;
                int current_min= std::stoi(tick[cnt].UpdateTime.substr(3,2))+1;
                int current_sec= std::stoi(tick[cnt].UpdateTime.substr(6,2));
                if(current_hour==23&&current_min==60)
                {
                    current_hour=0;
                    current_min=0;
                }
                else if(current_hour==02&&current_min>=31)
                {
                    current_hour=02;
                    current_min=30;
                }
                else if(current_hour==20&&current_min>=50)
                {
                    current_hour=21;
                    current_min=1;
                }
                else if(current_hour==21&&current_min==0)
                {
                    current_hour=21;
                    current_min=1;
                }
                else if(current_hour==8&&current_min>=55)
                {
                    current_hour=9;
                    current_min=1;
                }
                else if(current_hour==9&&current_min==0)
                {
                    current_hour=9;
                    current_min=1;
                }
                else if(current_hour==23&&current_min==01)
                {
                    current_hour=23;
                    current_min=0;
                }
                else if(current_hour==10&&current_min==16)
                {
                    current_hour=10;
                    current_min=15;
                }
                else if(current_hour==11&&current_min==31)
                {
                    current_hour=11;
                    current_min=30;
                }
                else if(current_hour==15&&current_min==1)
                {
                    current_hour=15;
                    current_min=0;
                }
                if(current_min>=60)
                {
                    current_min-=60;
                    current_hour+=1;
                }
                if(last_min==-1)
                {
                    last_min=current_min;
                }
                if(last_hour==-1)
                {
                    last_hour=current_hour;
                }
                data[current_hour][current_min].push_back(tick[cnt].LastPrice);
                if(current_hour==21&&current_min==1)
                {
                    current_volume=0;
                    last_volume=0;
                    last_min_total_volume=0;

                }
                if(data[current_hour][current_min].size()>0)
                {

                    double open=data[current_hour][current_min][0];
                    double close=data[current_hour][current_min][data[current_hour][current_min].size()-1];
                    double high=-1;
                    double low=10000000;
                    int volumen=0;

                    if(current_min!=last_min&&current_min==1)
                    {
                        open_hour=0;
                        close_hour=0;
                        low_hour=1000000;
                        high_hour=-1;
                    }
                    if(current_min!=last_min&&(current_hour==13&&current_min==31))
                    {
                        open_hour=0;
                        close_hour=0;
                        low_hour=1000000;
                        high_hour=-1;
                    }
                    if(current_min==1)
                    {
                        open_hour=open;
                    }
                    else if(current_min!=last_min&&(current_hour==13&&current_min==31))
                    {
                        open_hour=open;
                    }
                    //else if (current_min==0)
                    {
                        close_hour=close;
                    }

                    for (int k=0;k<data[current_hour][current_min].size();++k)
                    {
                        if(data[current_hour][current_min][k]<low)
                        {
                            low=data[current_hour][current_min][k];
                        }
                        if(data[current_hour][current_min][k]>high)
                        {
                            high=data[current_hour][current_min][k];
                        }
                        if(data[current_hour][current_min][k]<low_hour)
                        {
                            low_hour=data[current_hour][current_min][k];
                        }
                        if(data[current_hour][current_min][k]>high_hour)
                        {
                            high_hour=data[current_hour][current_min][k];
                        }
                    }
                    test_kindle.TradingDay=tick[cnt].TradingDay;
                    test_kindle.open=open;
                    test_kindle.close=close;
                    test_kindle.high=high;
                    test_kindle.low=low;

                    test_kindle_hour.TradingDay=tick[cnt].TradingDay;
                    test_kindle_hour.open=open_hour;
                    test_kindle_hour.close=close_hour;
                    test_kindle_hour.high=high_hour;
                    test_kindle_hour.low=low_hour;

                    std::string time_hour_min="";
                    time_hour_min=last_hour<10?time_hour_min+"0"+std::to_string(last_hour)+":":time_hour_min+std::to_string(last_hour)+":";
                    time_hour_min=last_min<10?time_hour_min+"0"+std::to_string(last_min):time_hour_min+std::to_string(last_min);
                    test_kindle.UpdateTime=time_hour_min;
                    test_kindle.InstrumentID=tick[0].InstrumentID;

                    test_kindle_hour.UpdateTime=time_hour_min;
                    test_kindle_hour.InstrumentID=tick[0].InstrumentID;

                    if(kindle_hour.size()==0)
                    {
                        kindle_hour.push_back(test_kindle_hour);
                    }
                    else if(current_min!=last_min&& current_min==1)
                    {
                        kindle_hour.push_back(test_kindle_hour);
                    }
                    else if(current_min!=last_min&& (current_hour==13&&current_min==31))
                    {
                        kindle_hour.push_back(test_kindle_hour);
                    }
                    else
                    {
                        kindle_hour.pop_back();
                        kindle_hour.push_back(test_kindle_hour);
                    }

                    if(cnt==0)
                    {
                        close_price_array.push_back(tick[cnt].LastPrice);
                        current_volume=test_tick.Volume;
                        volume.push_back(current_volume-last_volume);
                    }
                    if(last_min==current_min)
                    {
                        close_price_array.pop_back();
                        volume.pop_back();
                        close_price_array.push_back(tick[cnt].LastPrice);
                        current_volume=test_tick.Volume;
                        volume.push_back(current_volume-last_volume);
                    }
                    else if(last_min!=current_min)
                    {
                        close_price_array.push_back(tick[cnt].LastPrice);
                        last_min_total_volume=current_volume;
                        current_volume=test_tick.Volume;
                        volume.push_back(current_volume-last_volume);
                        last_volume=last_min_total_volume;
                    }
                    if(cnt==0)
                    {
                        quota_calculate_ma();
                        quota_init_zero();
                        quota_save_result();
                    }
                    if(close_price_array.size()==1)
                    {
                        quota_vector_popback();
                        quota_calculate_ma();
                        quota_init_zero();
                        quota_save_result();
                    }
                    if(close_price_array.size()>1&&last_min==current_min)
                    {
                        quota_vector_popback();
                        quota_calculate_ma();
                        quota_calculate_indicators();
                        quota_save_result();
                    }
                    if(close_price_array.size()>1&&last_min!=current_min)
                    {
                        quota_calculate_ma();
                        quota_calculate_indicators();
                        quota_save_result();
                    }

                }
                if(last_min!=current_min)
                {
                    data[last_hour][last_min].clear();
                    last_min=current_min;
                    last_hour=current_hour;
                    //volume.push_back(current_volume-last_volume);
                }
                cnt++;
                row_cnt++;

#if ROLLBACK
                if(kindle_hour.size()>50)
                {


                    if(global_order.TradingDay==test_tick.TradingDay)
                    {
                        ;
                    }
                    else if(global_order.TradingDay!=test_tick.TradingDay)
                    {
                        if(global_order.direction!=0&& global_order.open_flag==false)
                        {
                            global_order.direction=0;
                            global_order.TradingDay=test_tick.TradingDay;
                        }
                    }
                    if(global_order.direction==0&&global_order.open_flag==false)
                    {
                        query_array=Bill(kindle_hour,test_tick.LastPrice,query_array);
                        if(int(query_array[0])!=-1&&int(query_array[1])!=-1)
                        {
                            if(int(query_array[0])==0)
                            {
                                test1=int(query_array[1]);
                                test2=int(query_array[2]);
                                test3=int(query_array[3]);
                                test4=int(query_array[4]);
                                query_array[1]=test1;
                                query_array[2]=test2;
                                query_array[3]=test3;
                                query_array[4]=test4;
                                global_order.direction=1;
                                global_order.open_flag=false;
                                global_order.volume=int(query_array[4]);
                                std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"准备开多"<<global_order.volume<<"手 "<<query_array[1]<<"\t"<< query_array[2]<<"\t"<<query_array[3]<<std::endl;;
                            }
                            else if(int (query_array[0]==1))
                            {
                                test1=int(query_array[1]);
                                test2=int(query_array[2]);
                                test3=int(query_array[3]);
                                query_array[1]=test1;
                                query_array[2]=test2;
                                query_array[3]=test3;
                                global_order.direction=-1;
                                global_order.open_flag=false;
                                global_order.volume=int(query_array[4]);
                                std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"准备开空"<<global_order.volume<<"手 "<<query_array[1]<<"\t"<< query_array[2]<<"\t"<<query_array[3]<<std::endl;;
                            }
                        }
                    }
                    if(global_order.direction!=0)
                    {
                        if(global_order.open_flag==false)
                        {
                            if((global_order.direction==1)&&(global_order.open_flag==false) &&(test_tick.LastPrice <=query_array[1]))
                            {
                                query_array[1]=test_tick.LastPrice;
                                query_array[2]=query_array[1]-query_array[3];
                                global_order.open_price=query_array[1];
                                global_order.loss_price=query_array[2];
                                global_order.segment=query_array[3];
                                if(int(query_array[0])==int(query_array[5]))
                                {

                                }
                                std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"成功开多"<<global_order.volume<<"手 "<< global_order.open_price<<"\t"<< global_order.loss_price<<"\t"<<global_order.segment<<std::endl;;
                                global_order.highest_price=-1<<30;
                                global_order.lowest_price=1<<30;
                                global_order.open_flag=true;
                            }
                            else if ((global_order.direction==-1)&&(global_order.open_flag==false) &&(test_tick.LastPrice >=query_array[1]))
                            {
                                query_array[1]=test_tick.LastPrice;
                                query_array[2]=query_array[1]+query_array[3];
                                global_order.open_price=query_array[1];
                                global_order.loss_price=query_array[2];
                                global_order.segment=query_array[3];
                                global_order.open_flag=true;
                                std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"成功开空"<<global_order.volume<<"手 "<< global_order.open_price<<"\t"<< global_order.loss_price<<"\t"<<global_order.segment<<std::endl;;
                                global_order.highest_price=-1<<30;
                                global_order.lowest_price=1<<30;
                            }
                        }
                        if( global_order.open_flag==true&& global_order.direction!=0)
                        {
                            if(global_order.direction==1 &&test_tick.LastPrice<=global_order.loss_price)
                            {
                                if(test_tick.LastPrice-global_order.open_price>0)
                                {
                                    std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"开多止损下移盈利\t"<<global_order.volume*(test_tick.LastPrice-global_order.open_price)*10<<"\t"<< global_order.open_price<<"\t"<< global_order.loss_price<<"\t"<<global_order.segment<<std::endl;;
                                    totalprofit+=global_order.volume*(test_tick.LastPrice-global_order.open_price)*10;
                                }
                                else
                                {
                                    std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"开多损失\t"<<global_order.volume*(global_order.open_price-test_tick.LastPrice)*10<<"\t"<< global_order.open_price<<"\t"<< global_order.loss_price<<"\t"<<global_order.segment<<std::endl;;
                                    totalprofit+=-1*global_order.volume*(global_order.open_price-test_tick.LastPrice)*10;
                                    shoushukuisun++;
                                    kaiduoshoushukuisun++;
                                    totalkuisun+=-1*global_order.volume*(global_order.open_price-test_tick.LastPrice)*10;
                                    kaiduokuisun+=-1*global_order.volume*(global_order.open_price-test_tick.LastPrice)*10;

                                }
                                global_order.stop_profit=0;
                                global_order.direction=0;
                                topprofit=0;
                                global_order.open_flag=false;
                            }
                            if(global_order.direction==-1 &&test_tick.LastPrice>=global_order.loss_price)
                            {
                                if(global_order.open_price-test_tick.LastPrice>0)
                                {
                                    std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"开空止损下移盈利\t"<<global_order.volume*(global_order.open_price-test_tick.LastPrice)*10<<"\t"<< global_order.open_price<<"\t"<< global_order.loss_price<<"\t"<<global_order.segment<<std::endl;;
                                    totalprofit+=global_order.volume*(global_order.open_price-test_tick.LastPrice)*10;
                                }
                                else
                                {
                                    std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"开空损失\t"<<global_order.volume*(test_tick.LastPrice-global_order.open_price)*10<<"\t"<< global_order.open_price<<"\t"<< global_order.loss_price<<"\t"<<global_order.segment<<std::endl;;
                                    totalprofit+=-1*global_order.volume*(test_tick.LastPrice-global_order.open_price)*10;
                                    shoushukuisun++;
                                    kaikongshoushukuisun++;
                                    totalkuisun+=-1*global_order.volume*(test_tick.LastPrice-global_order.open_price)*10;
                                    kaikongkuisun+=-1*global_order.volume*(test_tick.LastPrice-global_order.open_price)*10;

                                }

                                global_order.direction=0;
                                topprofit=0;
                                global_order.stop_profit=0;
                                global_order.open_flag=false;
                            }
                        }
                        if( global_order.open_flag==true&& global_order.direction==1)
                        {
                            topprofit=topprofit>test_tick.LastPrice-global_order.open_price?topprofit:test_tick.LastPrice-global_order.open_price;
                            global_order.highest_price=global_order.highest_price>test_tick.LastPrice?global_order.highest_price:test_tick.LastPrice;

                            if(global_order.open_price+Profit_Percent_Rate*global_order.segment<=global_order.highest_price)
                            {
                                //global_order.segment-=1;
                                //global_order.highest_price+=global_order.segment;
                                if( global_order.highest_price-Profit_Percent_Rate*global_order.segment>=test_tick.LastPrice)
                                {


                                    shoushuyingli++;
                                    kaiduoshoushuyingli++;
                                    totalyingli+=(global_order.volume*(test_tick.LastPrice-global_order.open_price)*10);
                                    kaiduoyingli+=(global_order.volume*(test_tick.LastPrice-global_order.open_price)*10);




                                    std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"开多盈利\t"<<(global_order.volume*(test_tick.LastPrice-global_order.open_price)*10)<<"\t"<< global_order.open_price<<"\t"<< global_order.loss_price<<"\t"<<global_order.segment<<std::endl;;
                                    totalprofit+=(global_order.volume*(test_tick.LastPrice-global_order.open_price)*10);
                                    topprofit=0;
                                    global_order.direction=0;
                                    global_order.open_price=0;
                                    global_order.loss_price=0;
                                    global_order.segment=0;
                                    global_order.stop_profit=0;
                                    global_order.open_flag=false;
                                    global_order.highest_price=-1<<30;
                                    global_order.lowest_price=1<<30;


                                    //std::cout<<"global_order.open_flag=="<<global_order.open_flag<<std::endl;
                                }
                            }

                        }
                        if( global_order.open_flag==true&& global_order.direction==-1)
                        {
                            topprofit=topprofit>global_order.open_price-test_tick.LastPrice?topprofit:global_order.open_price-test_tick.LastPrice;
                            global_order.lowest_price=global_order.lowest_price<test_tick.LastPrice?global_order.lowest_price:test_tick.LastPrice;
                           if(global_order.open_price-Profit_Percent_Rate*global_order.segment>=global_order.lowest_price)
                           {
                               if( global_order.lowest_price+Profit_Percent_Rate*global_order.segment<=test_tick.LastPrice)
                               {
                                   shoushuyingli++;
                                   kaikongshoushuyingli++;
                                   totalyingli+=(global_order.volume*(global_order.open_price-test_tick.LastPrice)*10);
                                   kaikongyingli+=(global_order.volume*(global_order.open_price-test_tick.LastPrice)*10);

                                   std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"开空盈利\t"<<(global_order.volume*(global_order.open_price-test_tick.LastPrice)*10)<<"\t"<< global_order.open_price<<"\t"<< global_order.loss_price<<"\t"<<global_order.segment<<std::endl;;
                                   totalprofit+=(global_order.volume*(global_order.open_price-test_tick.LastPrice)*10);
                                   topprofit=0;
                                   global_order.direction=0;
                                   global_order.open_price=0;
                                   global_order.loss_price=0;
                                   global_order.segment=0;
                                   global_order.open_flag=false;
                                   global_order.stop_profit=0;
                                   global_order.highest_price=-1<<30;
                                   global_order.lowest_price=1<<30;

                               }
                           }


                        }
                    }

                }

#endif
                }
            std::cout<<"totalprofit=\t"<<totalprofit<<std::endl;
            std::cout<<"profit：\t"<<totalyingli<<std::endl;
            std::cout<<"buy_profit：\t"<<kaiduoyingli<<std::endl;
            std::cout<<"sell_profit：\t"<<kaikongyingli<<std::endl;
            std::cout<<"loss：\t"<<totalkuisun<<std::endl;
            std::cout<<"buy_loss：\t"<<kaiduokuisun<<std::endl;
            std::cout<<"sell_loss：\t"<<kaikongkuisun<<std::endl;
            std::cout<<"profit_num：\t"<<shoushuyingli<<std::endl;
            std::cout<<"buy_profit_num：\t"<<kaiduoshoushuyingli<<std::endl;
            std::cout<<"sell_profit_num：\t"<<kaikongshoushuyingli<<std::endl;
            std::cout<<"loss_num：\t"<<shoushukuisun<<std::endl;
            std::cout<<"buy_loss_num：\t"<<kaiduoshoushukuisun<<std::endl;
            std::cout<<"sell_loss_num：\t"<<kaikongshoushukuisun<<std::endl;
            std::cout<<"success_rate：\t"<<1.0*shoushuyingli/(shoushuyingli+shoushukuisun)<<std::endl;
            if(totalyingli>=(-1*totalkuisun))
            {
                std::cout<<"profit_vs_loss：\t"<<1.0*totalyingli/(-1*totalkuisun)<<std::endl;
            }
            else if(totalyingli<(-1*totalkuisun))
            {
                std::cout<<"profit_vs_loss：\t"<<-1.0*totalyingli/(-1*totalkuisun)<<std::endl;
            }


            end=clock();
            std::cout<<"total="<<total<<std::endl;
            std::cout<<"valid cnt="<<cnt<<std::endl;
            std::cout<<"res="<<res<<std::endl;
            mysql_free_result(result);     //释放结果集所占用的内存
            mysql_close(&conn);          //关闭与mysql的连接
            if(res)
            {
                // printf("error\n");
            }
            else
            {
                //printf("OK\n");
            }
            std::cout<<"HERE"<<std::endl;

            std::cout<<"HERE"<<std::endl;
            std::cout<<(double)(end-start)/CLOCKS_PER_SEC*1000<<"ms"<<std::endl;
        }
    }


    else
    {
        std::cout<<mysql_error(&conn)<<std::endl;
        std::cout << "connect failed" << std::endl;
    }
    return ;
}
void CustomMdSpi::solve()
{
    std::cout<<"begin run solve"<<std::endl;
    print_time();
    pre_solve();
    int row_cnt=0;
    int last_min=-1;
    int last_hour=-1;
    int last_slice=-1;
    int last_volume=0;
    int current_volume=0;
    int last_min_total_volume=0;
    double topprofit=0;
    double totalprofit=0;
    double total=0;

    double open_hour=0;
    double close_hour=0;
    double low_hour=1000000;
    double high_hour=-1;

    global_order.req_open_price=0;
    global_order.req_close_price=0;
    global_order.open_price=0;
    global_order.close_price=0;
    global_order.direction=0;
    global_order.loss_price=0;
    global_order.segment=0;
    global_order.open_flag= false;
    global_order.TradingDay="0000";
    //global_order.last_second_price=0;
    int test1,test2,test3,test4;
    while(1)
    {
        BarData bardata =saveDataQueue.take();
        //std::cout<<"cnt="<<cnt<<std::endl;

        Tick test_tick;
        test_tick.TradingDay=bardata.TradingDay;
        test_tick.LastPrice=bardata.LastPrice;
        test_tick.Volume=bardata.Volume;
        test_tick.OpenInterest=bardata.OpenInterest;
        test_tick.UpdateTime=bardata.UpdateTime;
        test_tick.UpdateMillisec=bardata.UpdateMillisec;
        test_tick.InstrumentID=bardata.InstrumentID;
        test_tick.Volume=bardata.Volume;
        tick.push_back(test_tick);
        int current_hour= std::stoi(tick[cnt].UpdateTime.substr(0,2));;
        int current_min= std::stoi(tick[cnt].UpdateTime.substr(3,2))+1;
        int current_sec= std::stoi(tick[cnt].UpdateTime.substr(6,2));
        if(current_hour==23&&current_min==60)
        {
            current_hour=0;
            current_min=0;
        }
        else if(current_hour==02&&current_min>=31)
        {
            current_hour=02;
            current_min=30;
        }
        else if(current_hour==20&&current_min>=50)
        {
            current_hour=21;
            current_min=1;
        }
        else if(current_hour==21&&current_min==0)
        {
            current_hour=21;
            current_min=1;
        }
        else if(current_hour==8&&current_min>=55)
        {
            current_hour=9;
            current_min=1;
        }
        else if(current_hour==9&&current_min==0)
        {
            current_hour=9;
            current_min=1;
        }
        else if(current_hour==23&&current_min==01)
        {
            current_hour=23;
            current_min=0;
        }
        else if(current_hour==10&&current_min==16)
        {
            current_hour=10;
            current_min=15;
        }
        else if(current_hour==11&&current_min==31)
        {
            current_hour=11;
            current_min=30;
        }
        else if(current_hour==15&&current_min==1)
        {
            current_hour=15;
            current_min=0;
        }
        if(current_min>=60)
        {
            current_min-=60;
            current_hour+=1;
        }
        if(last_min==-1)
        {
            last_min=current_min;
        }
        if(last_hour==-1)
        {
            last_hour=current_hour;
        }
        data[current_hour][current_min].push_back(tick[cnt].LastPrice);
        if(current_hour==21&&current_min==1)
        {
            current_volume=0;
            last_volume=0;
            last_min_total_volume=0;

        }
        if(data[current_hour][current_min].size()>0)
        {
            double open=data[current_hour][current_min][0];
            double close=data[current_hour][current_min][data[current_hour][current_min].size()-1];
            double high=-1;
            double low=10000000;


            if(current_min!=last_min&&current_min==1)
            {
                open_hour=0;
                close_hour=0;
                low_hour=1000000;
                high_hour=-1;
            }
            if(current_min!=last_min&&(current_hour==13&&current_min==31))
            {
                open_hour=0;
                close_hour=0;
                low_hour=1000000;
                high_hour=-1;
            }
            if(current_min==1)
            {
                open_hour=open;
            }
            else if(current_min!=last_min&&(current_hour==13&&current_min==31))
            {
                open_hour=open;
            }
            //else if (current_min==0)
            {
                close_hour=close;
            }

            for (int k=0;k<data[current_hour][current_min].size();++k)
            {
                if(data[current_hour][current_min][k]<low)
                {
                    low=data[current_hour][current_min][k];
                }
                if(data[current_hour][current_min][k]>high)
                {
                    high=data[current_hour][current_min][k];
                }
                if(data[current_hour][current_min][k]<low_hour)
                {
                    low_hour=data[current_hour][current_min][k];
                }
                if(data[current_hour][current_min][k]>high_hour)
                {
                    high_hour=data[current_hour][current_min][k];
                }
            }
            test_kindle.TradingDay=tick[cnt].TradingDay;
            test_kindle.open=open;
            test_kindle.close=close;
            test_kindle.high=high;
            test_kindle.low=low;

            test_kindle_hour.TradingDay=tick[cnt].TradingDay;
            test_kindle_hour.open=open_hour;
            test_kindle_hour.close=close_hour;
            test_kindle_hour.high=high_hour;
            test_kindle_hour.low=low_hour;

            std::string time_hour_min="";
            time_hour_min=last_hour<10?time_hour_min+"0"+std::to_string(last_hour)+":":time_hour_min+std::to_string(last_hour)+":";
            time_hour_min=last_min<10?time_hour_min+"0"+std::to_string(last_min):time_hour_min+std::to_string(last_min);
            test_kindle.UpdateTime=time_hour_min;
            test_kindle.InstrumentID=tick[0].InstrumentID;

            test_kindle_hour.UpdateTime=time_hour_min;
            test_kindle_hour.InstrumentID=tick[0].InstrumentID;

            if(kindle_hour.size()==0)
            {
                kindle_hour.push_back(test_kindle_hour);
            }
            else if(current_min!=last_min&& current_min==1)
            {
                kindle_hour.push_back(test_kindle_hour);
            }
            else if(current_min!=last_min&& (current_hour==13&&current_min==31))
            {
                kindle_hour.push_back(test_kindle_hour);
            }
            else
            {
                kindle_hour.pop_back();
                kindle_hour.push_back(test_kindle_hour);
            }

            if(cnt==0)
            {
                close_price_array.push_back(tick[cnt].LastPrice);
                current_volume=test_tick.Volume;
                volume.push_back(current_volume-last_volume);
            }
            if(last_min==current_min)
            {
                close_price_array.pop_back();
                volume.pop_back();
                close_price_array.push_back(tick[cnt].LastPrice);
                current_volume=test_tick.Volume;
                volume.push_back(current_volume-last_volume);
            }
            else if(last_min!=current_min)
            {
                close_price_array.push_back(tick[cnt].LastPrice);
                last_min_total_volume=current_volume;
                current_volume=test_tick.Volume;
                volume.push_back(current_volume-last_volume);
                last_volume=last_min_total_volume;
            }
            if(cnt==0)
            {
                quota_calculate_ma();
                quota_init_zero();
                quota_save_result();
            }
            if(close_price_array.size()==1)
            {
                quota_vector_popback();
                quota_calculate_ma();
                quota_init_zero();
                quota_save_result();
            }
            if(close_price_array.size()>1&&last_min==current_min)
            {
                quota_vector_popback();
                quota_calculate_ma();
                quota_calculate_indicators();
                quota_save_result();
            }
            if(close_price_array.size()>1&&last_min!=current_min)
            {
                quota_calculate_ma();
                quota_calculate_indicators();
                quota_save_result();
            }
        }


        if(last_min!=current_min)
        {
            data[last_hour][last_min].clear();
            last_min=current_min;
            last_hour=current_hour;
        }


#if CONTINUE
       // std::cout<<"kindle_hour.size="<<kindle_hour.size()<<std::endl;
        if(kindle_hour.size()>50)
        {
            if(global_order.TradingDay==test_tick.TradingDay)
            {
                ;
            }
            else if(global_order.TradingDay!=test_tick.TradingDay)
            {
                if(global_order.direction==0&&global_order.open_flag==false)
                {
                    global_order.TradingDay=test_tick.TradingDay;
                }
                else if(global_order.direction!=0&& global_order.open_flag==false)
                {
                    global_order.direction=0;
                    global_order.TradingDay=test_tick.TradingDay;
                }
            }
            if(global_order.direction==0&&global_order.open_flag==false)
            {
                std::string hour=test_tick.UpdateTime.substr(0,2);
                std::string min=test_tick.UpdateTime.substr(3,2);
                //std::cout<<"hour="<<hour<<" min="<<min<<std::endl;
                if((hour=="20"&&min=="59")||(hour=="08"&&min=="59"))
                {
                    std::cout<<test_tick.UpdateTime<<" 未开盘，不进行下单操作"<<std::endl;
                }
                else
                {
                    query_array=Bill(kindle_hour,test_tick.LastPrice,query_array);
                    if(int(query_array[0])!=-1&&int(query_array[1])!=-1)
                    {
                        if(int(query_array[0])==0)
                        {
                            test1=int(query_array[1]);
                            test2=int(query_array[2]);
                            test3=int(query_array[3]);
                            query_array[1]=test1;
                            query_array[2]=test2;
                            query_array[3]=test3;
                            global_order.direction=1;
                            global_order.open_flag=false;
                            global_order.req_open_price=query_array[1];
                            global_order.loss_price=query_array[2];
                            global_order.segment=query_array[3];
                            global_order.highest_price=-1<<30;
                            global_order.lowest_price=1<<30;
                            TThostFtdcInstrumentIDType instrument;
                            strcpy(instrument,test_tick.InstrumentID.c_str());
                            pTradeSpi->reqOrderInsert( instrument,THOST_FTDC_D_Buy,THOST_FTDC_OF_Open, global_order.req_open_price);
                            std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"准备开多"<<query_array[1]<<"\t"<< query_array[2]<<"\t"<<query_array[3]<<std::endl;;
                        }
                        else if(int (query_array[0]==1))
                        {
                            test1=int(query_array[1]);
                            test2=int(query_array[2]);
                            test3=int(query_array[3]);
                            query_array[1]=test1;
                            query_array[2]=test2;
                            query_array[3]=test3;
                            global_order.direction=-1;
                            global_order.open_flag=false;
                            global_order.req_open_price=query_array[1];
                            global_order.loss_price=query_array[2];
                            global_order.segment=query_array[3];
                            global_order.highest_price=-1<<30;
                            global_order.lowest_price=1<<30;
                            TThostFtdcInstrumentIDType instrument;
                            strcpy(instrument,test_tick.InstrumentID.c_str());
                            pTradeSpi->reqOrderInsert( instrument,THOST_FTDC_D_Sell,THOST_FTDC_OF_Open, global_order.req_open_price);
                            std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"准备开空"<<query_array[1]<<"\t"<< query_array[2]<<"\t"<<query_array[3]<<std::endl;;
                        }
                    }
                }

            }
            if(global_order.direction!=0)
            {
                if( global_order.open_flag==true&& global_order.direction!=0)
                {
                    if(global_order.direction==1 &&test_tick.LastPrice<=global_order.loss_price)
                    {

                        TThostFtdcInstrumentIDType instrument;
                        strcpy(instrument,test_tick.InstrumentID.c_str());
                        global_order.req_close_price=test_tick.LastPrice;
                        pTradeSpi->reqOrderInsert( instrument,THOST_FTDC_D_Sell,THOST_FTDC_OF_Close, global_order.req_close_price);
                        std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"准备止损多单"<<query_array[1]<<"\t"<< query_array[2]<<"\t"<<query_array[3]<<std::endl;;
                    }
                    if(global_order.direction==-1 &&test_tick.LastPrice>=global_order.loss_price)
                    {
                        TThostFtdcInstrumentIDType instrument;
                        strcpy(instrument,test_tick.InstrumentID.c_str());
                        global_order.req_close_price=test_tick.LastPrice;
                        pTradeSpi->reqOrderInsert( instrument,THOST_FTDC_D_Buy,THOST_FTDC_OF_Close, global_order.req_close_price);
                        std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"准备止损空单"<<query_array[1]<<"\t"<< query_array[2]<<"\t"<<query_array[3]<<std::endl;;
                    }
                }
                if( global_order.open_flag==true&& global_order.direction==1)
                {
                    global_order.highest_price=global_order.highest_price>test_tick.LastPrice?global_order.highest_price:test_tick.LastPrice;
                    if(global_order.open_price+Profit_Percent_Rate*global_order.segment<=global_order.highest_price)
                    {
                        if( global_order.highest_price-Profit_Percent_Rate*global_order.segment>=test_tick.LastPrice)
                        {
                            TThostFtdcInstrumentIDType instrument;
                            strcpy(instrument,test_tick.InstrumentID.c_str());
                            global_order.req_close_price=test_tick.LastPrice;
                            global_order.direction=0;
                            pTradeSpi->reqOrderInsert( instrument,THOST_FTDC_D_Sell,THOST_FTDC_OF_Close, global_order.req_close_price);
                            std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"准备止盈多单"<<query_array[1]<<"\t"<< query_array[2]<<"\t"<<query_array[3]<<std::endl;;
                        }
                    }
                }
                if( global_order.open_flag==true&& global_order.direction==-1)
                {
                    global_order.lowest_price=global_order.lowest_price<test_tick.LastPrice?global_order.lowest_price:test_tick.LastPrice;

                    if(global_order.open_price-Profit_Percent_Rate*global_order.segment>=global_order.lowest_price)
                    {
                        if( global_order.lowest_price+Profit_Percent_Rate*global_order.segment<=test_tick.LastPrice)
                        {
                            TThostFtdcInstrumentIDType instrument;
                            strcpy(instrument,test_tick.InstrumentID.c_str());
                            global_order.req_close_price=test_tick.LastPrice;
                            global_order.direction=0;
                            pTradeSpi->reqOrderInsert( instrument,THOST_FTDC_D_Buy,THOST_FTDC_OF_Close, global_order.req_close_price);
                            std::cout<<test_tick.TradingDay<<"\t"<<test_tick.InstrumentID<<"\t"<<test_tick.UpdateTime<<"\t"<<test_tick.UpdateMillisec<<"\t\t"<<test_tick.LastPrice<<"\t\t"<<"准备止盈空单"<<query_array[1]<<"\t"<< query_array[2]<<"\t"<<query_array[3]<<std::endl;;
                        }
                    }
                }
            }

        }
#endif
        cnt++;
        row_cnt++;
    }
}
void CustomMdSpi::quota_vector_popback()
{
    ma.pop_back();
    ma_80.pop_back();
    ma_360.pop_back();

    ema_fast_period.pop_back();
    ema_slow_period.pop_back();
    diff.pop_back();
    dea.pop_back();
    macd.pop_back();

    rsi_fast_period_up.pop_back();
    rsi_mid_period_up.pop_back();
    rsi_slow_period_up.pop_back();
    rsi_fast_period_down.pop_back();
    rsi_mid_period_down.pop_back();
    rsi_slow_period_down.pop_back();
    rsi_fast_period_smma_up.pop_back();
    rsi_mid_period_smma_up.pop_back();
    rsi_slow_period_smma_up.pop_back();
    rsi_fast_period_smma_down.pop_back();
    rsi_mid_period_smma_down.pop_back();
    rsi_slow_period_smma_down.pop_back();
    rsi_fast_period_rs.pop_back() ;
    rsi_mid_period_rs.pop_back();
    rsi_slow_period_rs.pop_back();

    rsi_fast_period_rsi.pop_back();
    rsi_mid_period_rsi.pop_back();
    rsi_slow_period_rsi.pop_back();

    kindle.pop_back();
    return ;
}
void CustomMdSpi::quota_calculate_ma()
{
    double ma_sum=0,ma_80_sum=0,ma_360_sum=0;
    if(close_price_array.size()>=10)
    {
        for(int i=close_price_array.size()-10;i<close_price_array.size();++i)
            ma_sum+=close_price_array[i];
    }
    else if(close_price_array.size()<10)
    {
        for(int i=0;i<close_price_array.size();++i)
            ma_sum+=close_price_array[i];
        ma_sum+=(10-close_price_array.size())*close_price_array[close_price_array.size()-1];
    }
    ma.push_back(ma_sum/10);
    if(close_price_array.size()>=80)
    {
        for(int i=close_price_array.size()-80;i<close_price_array.size();++i)
        {
            ma_80_sum+=close_price_array[i];
            //std::cout<<"i="<<i<<" ma_80_sum="<<ma_80_sum<<" close price[i]="<<close_price_array[i]<<std::endl;
        }


    }
    else if(close_price_array.size()<80)
    {
        for(int i=0;i<close_price_array.size();++i)
        {
            ma_80_sum+=close_price_array[i];
            //std::cout<<"i="<<i<<" ma_80_sum="<<ma_80_sum<<" close price[i]="<<close_price_array[i]<<std::endl;
        }

        ma_80_sum+=(80-close_price_array.size())*close_price_array[close_price_array.size()-1];
        //std::cout<<"ma_80_sum="<<ma_80_sum<<" left ="<<(80-close_price_array.size())<< " last="<<close_price_array[close_price_array.size()-1]<<"   ma_80_sum/80="<<ma_80_sum/80<<std::endl;
    }
    //std::cout<<"ma_80_sum/80="<<ma_80_sum/80<<std::endl;
    ma_80.push_back(ma_80_sum/80);
    if(close_price_array.size()>=360)
    {
        for(int i=close_price_array.size()-360;i<close_price_array.size();++i)
            ma_360_sum+=close_price_array[i];
    }
    else if(close_price_array.size()<360)
    {
        for(int i=0;i<close_price_array.size();++i)
            ma_360_sum+=close_price_array[i];
        ma_360_sum+=(360-close_price_array.size())*close_price_array[close_price_array.size()-1];
    }
    ma_360.push_back(ma_360_sum/360);
}
void CustomMdSpi::quota_init_zero()
{
    ema_fast_period.push_back(close_price_array[0]);
    ema_slow_period.push_back(close_price_array[0]);
    diff.push_back(ema_fast_period[0]-ema_slow_period[0]);
    dea.push_back(diff[0]);
    macd.push_back((diff[0]-dea[0])*2);
    double up=0;
    double down=0;
    rsi_fast_period_up.push_back(up);
    rsi_mid_period_up.push_back(up);
    rsi_slow_period_up.push_back(up);
    rsi_fast_period_down.push_back(down);
    rsi_mid_period_down.push_back(down);
    rsi_slow_period_down.push_back(down);
    rsi_fast_period_smma_up.push_back(rsi_fast_period_up[0]);
    rsi_mid_period_smma_up.push_back(rsi_mid_period_up[0]);
    rsi_slow_period_smma_up.push_back(rsi_slow_period_up[0]);
    rsi_fast_period_smma_down.push_back(rsi_fast_period_down[0]);
    rsi_mid_period_smma_down.push_back(rsi_mid_period_down[0]);
    rsi_slow_period_smma_down.push_back(rsi_slow_period_down[0]);
    rsi_fast_period_rs.push_back(rsi_fast_period_smma_up[0]/ rsi_fast_period_smma_down[0]) ;
    rsi_mid_period_rs.push_back(rsi_mid_period_smma_up[0]/ rsi_mid_period_smma_down[0]);
    rsi_slow_period_rs.push_back(rsi_slow_period_smma_up[0]/ rsi_slow_period_smma_down[0]);
    rsi_fast_period_rsi.push_back(100.0-(100.0/(1+rsi_fast_period_rs[0])));
    rsi_mid_period_rsi.push_back(100.0-(100.0/(1+rsi_mid_period_rs[0])));
    rsi_slow_period_rsi.push_back(100.0-(100.0/(1+rsi_slow_period_rs[0])));
    //cout<<"=="<<std::setw(10)<<std::left<<diff[diff.size()]<<"\t"<<std::setw(15)<<std::left<<dea[dea.size()]<<endl;
    return ;
}
void CustomMdSpi::quota_calculate_indicators()
{
    int cur=close_price_array.size()-1;
    int pre=close_price_array.size()-2;

    ema_fast_period.push_back(2*close_price_array[cur]/(macd_fast_period+1)+ema_fast_period[cur-1]*(macd_fast_period-1)/(macd_fast_period+1));
    ema_slow_period.push_back(2*close_price_array[cur]/(macd_slow_period+1)+ema_slow_period[cur-1]*(macd_slow_period-1)/(macd_slow_period+1));
    diff.push_back(ema_fast_period[cur]-ema_slow_period[cur]);
    dea.push_back(2*diff[cur]/(macd_signal_period+1)+dea[cur-1]*(macd_signal_period-1)/(macd_signal_period+1));
    macd.push_back((diff[cur]-dea[cur])*2);

    double up=(close_price_array[cur]-close_price_array[pre])>0?close_price_array[cur]-close_price_array[pre]:0;
    double down=(close_price_array[pre]-close_price_array[cur])>0?close_price_array[pre]-close_price_array[cur]:0;
    rsi_fast_period_up.push_back(up);
    rsi_mid_period_up.push_back(up);
    rsi_slow_period_up.push_back(up);
    rsi_fast_period_down.push_back(down);
    rsi_mid_period_down.push_back(down);
    rsi_slow_period_down.push_back(down);
    rsi_fast_period_smma_up.push_back(rsi_fast_period_up[cur]/rsi_fast_period+rsi_fast_period_smma_up[cur-1]*(rsi_fast_period-1)/rsi_fast_period);
    rsi_mid_period_smma_up.push_back(rsi_mid_period_up[cur]/rsi_mid_period+rsi_mid_period_smma_up[cur-1]*(rsi_mid_period-1)/rsi_mid_period);
    rsi_slow_period_smma_up.push_back(rsi_slow_period_up[cur]/rsi_slow_period+rsi_slow_period_smma_up[cur-1]*(rsi_slow_period-1)/rsi_slow_period);
    rsi_fast_period_smma_down.push_back(rsi_fast_period_down[cur]/rsi_fast_period+rsi_fast_period_smma_down[cur-1]*(rsi_fast_period-1)/rsi_fast_period);
    rsi_mid_period_smma_down.push_back(rsi_mid_period_down[cur]/rsi_mid_period+rsi_mid_period_smma_down[cur-1]*(rsi_mid_period-1)/rsi_mid_period);
    rsi_slow_period_smma_down.push_back(rsi_slow_period_down[cur]/rsi_slow_period+rsi_slow_period_smma_down[cur-1]*(rsi_slow_period-1)/rsi_slow_period);

    rsi_fast_period_rs.push_back(rsi_fast_period_smma_up[cur]/ rsi_fast_period_smma_down[cur]);
    rsi_mid_period_rs.push_back(rsi_mid_period_smma_up[cur]/ rsi_mid_period_smma_down[cur]);
    rsi_slow_period_rs.push_back(rsi_slow_period_smma_up[cur]/ rsi_slow_period_smma_down[cur]);

    rsi_fast_period_rsi.push_back(100.0-(100.0/(1+rsi_fast_period_rs[cur])));
    rsi_mid_period_rsi.push_back(100.0-(100.0/(1+rsi_mid_period_rs[cur])));
    rsi_slow_period_rsi.push_back(100.0-(100.0/(1+rsi_slow_period_rs[cur])));
    return ;
}
void CustomMdSpi::quota_save_result()
{
    test_kindle.MA=ma[close_price_array.size()-1];
    test_kindle.MA_80=ma_80[close_price_array.size()-1];
    test_kindle.MA_360=ma_360[close_price_array.size()-1];
    test_kindle.DIFF=diff[close_price_array.size()-1];
    test_kindle.DEA=dea[close_price_array.size()-1];
    test_kindle.MACD=macd[close_price_array.size()-1];
    test_kindle.RSI1=rsi_fast_period_rsi[close_price_array.size()-1];
    test_kindle.RSI2=rsi_mid_period_rsi[close_price_array.size()-1];
    test_kindle.RSI3=rsi_slow_period_rsi[close_price_array.size()-1];
    kindle.push_back(test_kindle);
}
double* Bill(std::vector<Kindle> kindle,double LastPrice,double *A)
{
    //此处为测试，根据需求自定义策略

    A[0]=-1;
    A[1]=-1;
    A[3]=0;
    A[4]=0;
    //A[5]=-1;
   
    int size=kindle.size();
    double shiti=0, shitihe=0, Kopen1=0, Kclos1=0, Kopen2=0, Kclos2=0;
    //最后一K线的最大值Kopen1和最小值Kclos1
    Kopen1 = (kindle[size - 2].open > kindle[size - 2].close) ? kindle[size - 2].open : kindle[size - 2].close;
    Kclos1 = (kindle[size - 2].open < kindle[size - 2].close) ? kindle[size - 2].open : kindle[size - 2].close;



    //方向和K实体的值。是否实体初值为0？////
    //
    if (kindle[size - 2].close > kindle[size - 2].open) {
        shiti = kindle[size - 2].close - kindle[size -2].open;//阳
        //std::cout<<"sun_shiti="<<shiti<<std::endl;
        A[0] = THOST_FTDC_D_Buy-'0';
    }
    else  if (kindle[size - 2].close < kindle[size - 2].open)
    {
        shiti = kindle[size - 2].open - kindle[size - 2].close;//阴
        //std::cout<<"cloudy_shiti="<<shiti<<std::endl;
        A[0] = THOST_FTDC_D_Sell-'0';
    }
    //前5K的最大Kopen2值和最小值Kopen2分别的和。
    double high=0;
    double low=0;
    for(int i=size-3;i>size-8;i--)
    {
       // std::cout<<"open"<<kindle[i].open<<" close"<<kindle[i].close<<" high"<<kindle[i].high<<" low"<<kindle[i].low<<std::endl;
        Kopen2 += (kindle[i].open > kindle[i].close) ? kindle[i].open : kindle[i].close;
        Kclos2 += (kindle[i].open < kindle[i].close) ? kindle[i].open : kindle[i].close;
        //high = (kindle[i].high > high) ? kindle[i].high : high;
        //low = (kindle[i].low < low) ? kindle[i].low : low;
        high += (kindle[i].high > kindle[i].low) ? kindle[i].high : kindle[i].low;
        low += (kindle[i].high < kindle[i].low) ? kindle[i].high : kindle[i].low;
    }
    //前5K的实体平均值。
    Kopen2 = Kopen2 / 5;
    Kclos2 = Kclos2 / 5;
    shitihe = Kopen2 - Kclos2;
    high/=5;
    low/=5;
    //std::cout<<"kopen1="<<Kopen1<<"kopen2="<<Kopen2<<"kclose1="<<Kclos1<<" Kclose2="<<Kclos2<<" shiti="<<shiti<<" shitihe="<<shitihe<<" highe="<<high<<" low="<<low<<std::endl;
    if ( A[0] == THOST_FTDC_D_Sell-'0') {//空
        //std::cout<<"kopen2="<<Kopen2<<" Kclose2="<<Kclos2<<" shiti="<<shiti<<" shitihe="<<shitihe<<std::endl;
        if (shiti/ shitihe>2) {
            //A[1] = 现价;
            A[1] = Kclos1+(Kopen2-Kclos2)*0.3;
            //A[1]=LastPrice;
            //A[2] =A[1]+high-low;
            A[2] =A[1]+(high-low)*0.8;
            //A[4] = Kclos1 * 0.991;
            A[3]=(high-low)*0.8;//振幅
            A[4]=1;
# if DETAILINFO
            //输出日志
            //方向K实体的值。是否实体初值为0？
            if (kindle[size - 2].close > kindle[size - 2].open) {
               // shiti = kindle[size - 2].close - kindle[size -2].open;//阳
                std::cout<<"sun_shiti="<<shiti<<std::endl;
                //A[0] = THOST_FTDC_D_Buy-'0';
            }
            else  if (kindle[size - 2].close < kindle[size - 2].open)
            {
                //shiti = kindle[size - 2].open - kindle[size - 2].close;//阴
                std::cout<<"cloudy_shiti="<<shiti<<std::endl;
                //A[0] = THOST_FTDC_D_Sell-'0';
            }

            for(int i=size-3;i>size-8;i--)
            {
                std::cout<<"open"<<kindle[i].open<<" close"<<kindle[i].close<<" high"<<kindle[i].high<<" low"<<kindle[i].low<<std::endl;
            }
            std::cout<<"kopen1="<<Kopen1<<"kopen2="<<Kopen2<<"kclose1="<<Kclos1<<" Kclose2="<<Kclos2<<" shiti="<<shiti<<" shitihe="<<shitihe<<" highe="<<high<<" low="<<low<<std::endl;
#endif
        }
    }
    else if(shiti/shitihe>2)
    {
       //buy
        //A[1] = 价格;

        //A[1] =  Kopen1-(Kopen1-Kclos1)*0.1;
        A[1] =  Kopen1-(Kopen2-Kclos2)*0.3;
        //A[1]=LastPrice;
        //A[2] = A[1]-(high-low);
        A[2] = A[1]-(high-low)*0.8;
        //A[3] = Kopen1 * 1.008;
        A[3]=(high-low)*0.8;//振幅
        A[4]=1;//num判断

# if DETAILINFO
        //输出日志
        //方向和K实体的值。是否实体初值为0？
        if (kindle[size - 2].close > kindle[size - 2].open) {
            // shiti = kindle[size - 2].close - kindle[size -2].open;//阳
            std::cout<<"sun_shiti="<<shiti<<std::endl;
            //A[0] = THOST_FTDC_D_Buy-'0';
        }
        else  if (kindle[size - 2].close < kindle[size - 2].open)
        {
            //shiti = kindle[size - 2].open - kindle[size - 2].close;//阴
            std::cout<<"cloudy_shiti="<<shiti<<std::endl;
            //A[0] = THOST_FTDC_D_Sell-'0';
        }

        for(int i=size-3;i>size-8;i--)
        {
            std::cout<<"open"<<kindle[i].open<<" close"<<kindle[i].close<<" high"<<kindle[i].high<<" low"<<kindle[i].low<<std::endl;
        }
        std::cout<<"kopen1="<<Kopen1<<"kopen2="<<Kopen2<<"kclose1="<<Kclos1<<" Kclose2="<<Kclos2<<" shiti="<<shiti<<" shitihe="<<shitihe<<" highe="<<high<<" low="<<low<<std::endl;
#endif
    }
    /*
    if (kindle[size - 30].close < kindle[size - 2].close)
    {
        A[4] = 1;
    }
    else if(kindle[size-30].close>kindle[size-2].close)
    {
        A[4] = 1;
    }
*/

    if(A[0]!=-1)
    {

        int j = 0, k = 0;
        for (int i = 0; i < 5; i++)
        {
            j += 6;
            if (kindle[size - 2 - j].close > kindle[size - 8 - j].close) {
                //buy

                k++;
            }
            else if (kindle[size - 2 - j].close < kindle[size - 8 - j].close) {
                k=0;
            }
            if (k >= 3)
            {
                A[4] = 2;
            }
            else {
                A[4] = 1;
            }
            //cout << "j==" << j << "\tk==" << k << "\tA4==" << A[4]<< "现在"<<kindle[size - 2 - j].close <<"以前"<< kindle[size - 7 - j].close << endl;
        }
    }


    //std::cout<<"A="<<A<<std::endl;
    return A;

}
