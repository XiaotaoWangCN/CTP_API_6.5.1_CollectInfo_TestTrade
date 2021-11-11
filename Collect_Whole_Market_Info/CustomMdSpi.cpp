#include <iostream>
#include <fstream>
#include <unordered_map>
#include "CustomMdSpi.h"
#include<cstring>

// 互斥量与条件变量

// ---- 全局参数声明 ---- //
extern CThostFtdcMdApi *g_pMdUserApi;
extern char gMdFrontAddr[];
extern TThostFtdcBrokerIDType gBrokerID;
extern TThostFtdcInvestorIDType gInvesterID;
extern TThostFtdcPasswordType gInvesterPassword;

BlockQueue<BarData > CustomMdSpi::marketDataQueue;
BlockQueue<BarData > CustomMdSpi::saveDataQueue;
BarData CustomMdSpi::bararray[SIZE];
int CustomMdSpi::instrument_cnt=0;
// ---- ctp_api回调函数 ---- //
// 连接成功应答
void CustomMdSpi::OnFrontConnected()
{
	// 开始登录
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq));
	strcpy(loginReq.BrokerID, gBrokerID);
	strcpy(loginReq.UserID, gInvesterID);
	strcpy(loginReq.Password, gInvesterPassword);
	static int requestID = 0; // 请求编号
	int rt = g_pMdUserApi->ReqUserLogin(&loginReq, requestID);
	//std::cout<<"HERE1";
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

/*
	// 计算实时k线
	std::string instrumentKey = std::string(pDepthMarketData->InstrumentID);
	if (g_KlineHash.find(instrumentKey) == g_KlineHash.end())
		g_KlineHash[instrumentKey] = TickToKlineHelper();
	g_KlineHash[instrumentKey].KLineFromRealtimeData(pDepthMarketData);
*/

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
std::string CustomMdSpi::get_year_mon()
{
    time_t tt;
    time( &tt );
    tt = tt +8*3600;  // transform the time zone
    tm* t= gmtime( &tt );
    //std::cout<<t->tm_hour<<"\t"<<t->tm_min<<"\t"<<t->tm_sec<<std::endl;
    int client_year=t->tm_year+1900;
    int client_mon=t->tm_mon+1;
    std::string year_mon="";
    year_mon+= std::to_string(client_year);
    year_mon+="-";
    year_mon+= std::to_string(client_mon);
    return year_mon;
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
                //bardata.BidPrice1=0;
               // flag=false;
            }
            if(bardata.AskPrice1>inf)
            {
		 bardata.AskPrice1=0;
                //bardata.AskPrice1=0;
               // flag=false;
            }
            if(flag==true)
            {
                if(ExchangeID.compare("SHFE")==0||ExchangeID.compare("CZCE")==0||ExchangeID.compare("INE")==0||ExchangeID.compare("DCE")==0)
                {
                    if((32100<=server_totalsec&&server_totalsec<=36900)||(37800<=server_totalsec&&server_totalsec<=41400)||(48600<=server_totalsec&&server_totalsec<=54000)||(75300<=server_totalsec&&server_totalsec<=82800)||(82800<=server_totalsec&&server_totalsec<=86400)||(0<=server_totalsec&&server_totalsec<=9000))
                    {
                        saveDataQueue.put(bardata);
                    }
                }
                else if (ExchangeID.compare("CFFEX")==0)
                {
                    if((32100<=server_totalsec&&server_totalsec<=41400)||(46800<=server_totalsec&&server_totalsec<=54900)||(75300<=server_totalsec&&server_totalsec<=82800)||(82800<=server_totalsec&&server_totalsec<=86400)||(0<=server_totalsec&&server_totalsec<=9000))
                    {
                        saveDataQueue.put(bardata);
                    }
                }
            }

        }
    }
}

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
                    std::ofstream fileout;
                    std::string error_filepath="./Error-";
                    error_filepath+=get_year_mon();
                    error_filepath+="-log.txt";
                    fileout.open(error_filepath,std::ofstream::app);
                    fileout<<test<<std::endl;
                    std::cout<<"Error see"<<error_filepath<<std::endl;
                    fileout.close();
                    //std::cout <<test << std::endl;


                   // print_mysql_error_time();
                   // std::cout <<test << std::endl;
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

