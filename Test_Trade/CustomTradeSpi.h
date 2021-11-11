#pragma once
// ---- 派生的交易类 ---- //
#ifndef V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_CUSTOMTRADESPI_H
#define V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_CUSTOMTRADESPI_H
#include "CTP_API/ThostFtdcTraderApi.h"
#include <map>
#include <iterator>
#include "Order.h"

class CustomTradeSpi : public CThostFtdcTraderSpi
{
// ---- ctp_api部分回调接口 ---- //
public:
    ///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
    void OnFrontConnected();
    //virtual int ReqAuthenticate(CThostFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID);
    virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

    ///登录请求响应
    void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///错误应答
    void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
    void OnFrontDisconnected(int nReason);

    ///心跳超时警告。当长时间未收到报文时，该方法被调用。
    void OnHeartBeatWarning(int nTimeLapse);

    ///登出请求响应
    void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///投资者结算结果确认响应
    void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///请求查询合约响应
    void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///请求查询资金账户响应
    void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///请求查询投资者持仓响应
    void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///请求查询投资者持仓明细响应
    void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///报单录入请求响应
    void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///报单操作请求响应
    void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///报单通知
    void OnRtnOrder(CThostFtdcOrderField *pOrder);

    ///成交通知
    void OnRtnTrade(CThostFtdcTradeField *pTrade);
    //int  ReqAuthenticate(CThostFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID) ;

    ///合约交易状态通知
     //void OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus);

// ---- 自定义函数 ---- //
public:
    bool loginFlag; // 登陆成功的标识
    void reqOrderInsert(char *instrumentID, TThostFtdcDirectionType direction, TThostFtdcOrderPriceTypeType combOffsetFlag,double limitPrice);
    static std::vector<std::string> instruments;
    static int requestID;
    static int orderRef;
private:
    void reqUserLogin(); // 登录请求
    void reqUserLogout(); // 登出请求
    void reqSettlementInfoConfirm(); // 投资者结果确认
    void reqQueryInstrument(); // 请求查询合约
    void reqQueryTradingAccount(); // 请求查询资金帐户
    void reqQueryInvestorPosition(); // 请求查询投资者持仓
    void reqQueryInvestorPositionDetail();// 请求查询投资者持仓明细

    void reqOrderAction(CThostFtdcOrderField *pOrder); // 请求报单操作
    bool isErrorRspInfo(CThostFtdcRspInfoField *pRspInfo); // 是否收到错误信息
    bool isMyOrder(CThostFtdcOrderField *pOrder); // 是否我的报单回报
    bool isTradingOrder(CThostFtdcOrderField *pOrder); // 是否正在交易的报单
    static void print_time();
    std::map<char,std::string> order_status_map;

};
#endif //V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_CUSTOMTRADESPI_H