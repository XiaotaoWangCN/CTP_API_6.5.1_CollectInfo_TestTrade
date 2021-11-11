//
// Created by boer on 2021/7/22.
//

#ifndef V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_TICK_H
#define V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_TICK_H


#include<iostream>
class Tick{
public:
    std::string TradingDay;
    double LastPrice;
    int Volume;
    int OpenInterest;
    std::string  UpdateTime;
    int UpdateMillisec;
    std::string  InstrumentID;
};
#endif //V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_TICK_H