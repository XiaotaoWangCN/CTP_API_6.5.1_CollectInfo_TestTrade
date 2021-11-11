//
// Created by boer on 2021/5/28.
//

#ifndef V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_BARDATA_H
#define V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_BARDATA_H


#include<iostream>
//TradingDay,InstrumentID,LastPrice,Volume,UpdateTime,UpdateMillisec,BidPrice1,BidVolume1,AskPrice1,AskVolume1,ActionDay
class BarData{
public:
    std::string	TradingDay;
    std::string	reserve1;
    std::string	ExchangeID;
    std::string	reserve2;
    double	LastPrice;
    double	PreSettlementPrice;
    double	PreClosePrice;
    double	PreOpenInterest;
    double	OpenPrice;
    double	HighestPrice;
    double	LowestPrice;
    int	Volume;
    double	Turnover;
    double	OpenInterest;
    double	ClosePrice;
    double	SettlementPrice;
    double	UpperLimitPrice;
    double	LowerLimitPrice;
    double	PreDelta;
    double	CurrDelta;
    std::string	UpdateTime;
    int	UpdateMillisec;
    double	BidPrice1;
    int	BidVolume1;
    double	AskPrice1;
    int	AskVolume1;
    double	BidPrice2;
    int	BidVolume2;
    double	AskPrice2;
    int	AskVolume2;
    double	BidPrice3;
    int	BidVolume3;
    double	AskPrice3;
    int	AskVolume3;
    double	BidPrice4;
    int	BidVolume4;
    double	AskPrice4;
    int	AskVolume4;
    double	BidPrice5;
    int	BidVolume5;
    double	AskPrice5;
    int	AskVolume5;
    double	AveragePrice;
    std::string	ActionDay;
    std::string	InstrumentID;
    std::string	ExchangeInstID;


};
#endif //V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_BARDATA_H
