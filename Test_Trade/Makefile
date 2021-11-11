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
