//
// Created by boer on 2021/7/22.
//

#ifndef V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_ORDER_H
#define V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_ORDER_H

class Order
{
public:
    int OrderSysID;
    int orderRef;
    std::string TradingDay;
    double req_open_price;
    double open_price;
    int direction;
    int volume;
    double loss_price;
    double segment;
    bool open_flag;
    double highest_price;
    double lowest_price;
    double req_close_price;
    double close_price;
    int stop_profit;

};
#endif //V6_5_1_20200908_API_TRADEAPI_SE_LINUX64_ORDER_H
