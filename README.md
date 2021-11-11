# CTP_API_6.5.1_Product_Version_CollectInfo_TestTrade
CTP_API_6.5.1_CollectInfo_TestTrade, Collect Whole Market Info and stored in DB and An Example of Test Trade

API can be downloaded from here http://www.sfit.com.cn/5_2_DocumentDown_6.htm (Version:v6.5.1_20200908 16:00:00)
Ofcourse,you can also download some mannual from such website and such can help you learn more info about API.

In Market Program,after Install Mysql and prepair dynamic links for CTP API
In Market Folder,just run make,the as follow 

TAG=main

OBJ= CustomMdSpi.o CustomTradeSpi.o main.o

CC:=g++

CFLAGS:=-g

INCLUDE=-I/usr/local/mysql/include

LIB=-L/usr/lib -lmysqlclient -lpthread -lthostmduserapi_se -lthosttraderapi_se

RM:=rm -f

#all:$(TAG)

$(TAG):$(OBJ)

	$(CC) $(CFLAGS) $^ -o $@  $(LIB) 

%.o:%.cpp

	$(CC) $(CFLAGS) -c $^ -o $@ $(INCLUDE) 

.PHONY:

clean:

	$(RM) $(OBJ)






In Test Trade Program,makefile are as follow

TAG=main

OBJ= CustomMdSpi.o CustomTradeSpi.o  main.o

CC:=g++

CFLAGS:=-g

INCLUDE=-I/usr/local/mysql/include

LIB=-L/usr/lib -lmysqlclient -lpthread -lthostmduserapi_se -lthosttraderapi_se  /usr/local/lib/libtinyxml2.a

RM:=rm -f

#all:$(TAG)

$(TAG):$(OBJ)

	$(CC) $(CFLAGS) $^ -o $@  $(LIB)

%.o:%.cpp

	$(CC) $(CFLAGS) -c $^ -o $@ $(INCLUDE)

.PHONY:

clean:

	$(RM) $(OBJ)



If your Mysql install dir was not the same as the Makefile,you should modify the Makefile In order to make sure they are same with your environment.

