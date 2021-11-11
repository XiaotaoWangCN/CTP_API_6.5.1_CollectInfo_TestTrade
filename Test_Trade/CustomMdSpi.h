#pragma once
#ifndef V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_CUSTOMDSPI_H
#define V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_CUSTOMDSPI_H
// ---- 派生的行情类 ---- //
#include <vector>
#include<map>
#include<queue>
#include<map>
#include<cstdio>
#include<fstream>
#include <condition_variable>
#include<mutex>
#include "CTP_API/ThostFtdcMdApi.h"
#include "BlockQueue.h"
#include "BarData.h"
#include <mysql.h>
#include "Kindle.h"
#include "Tick.h"
#include "Order.h"
#include "CustomTradeSpi.h"
class CustomMdSpi: public CThostFtdcMdSpi
{
	// ---- 继承自CTP父类的回调接口并实现 ---- //
public:
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	void OnFrontConnected();

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	void OnFrontDisconnected(int nReason);

	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	void OnHeartBeatWarning(int nTimeLapse);

	///登录请求响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///登出请求响应
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///错误应答
	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///订阅行情应答
	void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅行情应答
	void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///订阅询价应答
	void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅询价应答
	void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	///询价通知
	void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);
    void  static pre_write();
    void  static write();
    static void print_time();
    static void print_mysql_error_time();
    static void pre_solve();
    void static solve();
    static void quota_vector_popback();
    static void quota_calculate_ma();
    static void quota_init_zero();
    static void quota_calculate_indicators();
    static void quota_save_result();
    static BlockQueue<BarData> marketDataQueue;
    static BlockQueue<BarData> saveDataQueue;
    const static int SIZE=10000;
    static BarData bararray[SIZE];
    const static int macd_fast_period=12;
    const static int macd_slow_period=26;
    const static int macd_signal_period=9;
    const static int rsi_fast_period=6;
    const static int rsi_mid_period=12;
    const static int rsi_slow_period=24;
    static std::vector<Kindle> kindle,kindle_hour;
    static std::vector<Tick> tick;
    static std::vector<double> ema_fast_period,ema_slow_period;
    static std::vector<int> volume;
    static std::vector<double> ma,ma_80,ma_360;
    static std::vector<double>diff,dea,macd;
    static std::vector <double> close_price_array;
    static std::vector<std::vector<std::vector<int> > > data;//创建2个vector<vector<int> >类型的数组
    static std::vector<double> rsi_fast_period_up,rsi_mid_period_up,rsi_slow_period_up;
    static std::vector<double> rsi_fast_period_down,rsi_mid_period_down,rsi_slow_period_down;
    static std::vector<double> rsi_fast_period_smma_up,rsi_mid_period_smma_up,rsi_slow_period_smma_up;
    static std::vector<double> rsi_fast_period_smma_down,rsi_mid_period_smma_down,rsi_slow_period_smma_down;
    static std::vector<double> rsi_fast_period_rs,rsi_mid_period_rs,rsi_slow_period_rs;
    static std::vector<double> rsi_fast_period_rsi,rsi_mid_period_rsi,rsi_slow_period_rsi;
    static Kindle test_kindle,test_kindle_hour;
    static int cnt;
    static int requestID;
    static int instrument_cnt;

};
//
#endif //V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_CUSTOMDSPI_H

