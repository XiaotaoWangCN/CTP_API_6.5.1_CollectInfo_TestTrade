//
// Created by boer on 2021/7/22.
//

#ifndef V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_KINDLE_H
#define V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_KINDLE_H

#endif //V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_KINDLE_H
#include<iostream>
class Kindle
{
public:
    std::string TradingDay;
    std::string UpdateTime;
    double open;
    double close;
    double high;
    double low;
    double MA;
    double DIFF;
    double DEA;
    double MACD;
    double RSI1;
    double RSI2;
    double RSI3;
    std::string InstrumentID;
};