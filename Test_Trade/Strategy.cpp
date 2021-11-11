//
// Created by boer on 2021/8/23.
//
#include "Strategy.h"
//#include "CustomTradeSpi.h"

//extern CustomTradeSpi *pTradeSpi ;
//
// Created by boer on 2021/8/23.
//
#include "Strategy.h"
#include "CustomTradeSpi.h"

//需要注释掉
///买
//#define THOST_FTDC_D_Buy '0'
///卖
//#define THOST_FTDC_D_Sell '1'

///开仓
//#define THOST_FTDC_OF_Open '0'
///平仓
//#define THOST_FTDC_OF_Close '1'

extern CustomTradeSpi *pTradeSpi ;
double* Bill(vector<Kindle> kindle,double LastPrice)
{
    //假设开仓已经成交

    double A[6];
    A[0]=-1;
    //开仓方向，开仓价格，止损价格，高点价格(多止盈价)，低点价格(空止盈价)
    // 挂止损单
    int size=kindle.size();
    double shiti, shitihe, Kopen1, Kclos1, Kopen2, Kclos2;
    //最后一根K线的最大值Kopen1和最小值Kclos1
    Kopen1 = (kindle[size - 1].open > kindle[size - 1].close) ? kindle[size - 1].open : kindle[size - 1].close;
    Kclos1 = (kindle[size - 1].open < kindle[size - 1].close) ? kindle[size - 1].close : kindle[size - 1].open;
    //方向是多还是空，和K线实体的值。
    if (kindle[size - 1].close > kindle[size - 1].open) {
        shiti = kindle[size - 1].close - kindle[size - 1].open;//阳线
        A[0] = THOST_FTDC_D_Buy;
    }
    else {
        shiti = kindle[size - 1].open - kindle[size - 1].close;//阴线
        A[0] = THOST_FTDC_D_Sell;
    }
    //前5根K线的最大Kopen2值和最小值Kopen2分别的和。
    for(int i=size-2;i>size-7;i--){
        Kopen2 += (kindle[i].open > kindle[i].close) ? kindle[i].open : kindle[i].close;
        Kclos2 += (kindle[i].open < kindle[i].close) ? kindle[i].close : kindle[i].open;
    }
    //前5根K线的实体平均值。
    Kopen2 = Kopen2 / 5;
    Kclos2 = Kclos2 / 5;
    shitihe = Kopen2 - Kclos2;
    if (A[0]) {//空
        if (Kopen1 > Kopen2&& Kclos1 <Kclos2 && shiti/ shitihe>1.3) {
            //A[1] = 现价;
            A[1] = LastPrice;
            A[2] = Kopen1;
            A[4] = Kclos1 * 0.7;
        }
    }
    else if(Kopen1<Kopen2&&Kclos1>Kclos2&&shitihe>1.3)
    {
        //多
        //A[1] = 现价;

        A[1] = LastPrice;
        A[2] = Kclos1;
        A[3] = Kopen1 * 1.3;
    }


    return A;

}

