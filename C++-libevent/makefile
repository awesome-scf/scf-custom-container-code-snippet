
CC=g++

OBJS = $(wildcard *.cpp)

TARGET = http-server

all:
	$(CC) $(OBJS) -g -std=c++11 -I/usr/include -I cos-cpp-sdk-v5/include -I /usr/local/include  -L/usr/lib64 -Lcos-cpp-sdk-v5/lib -L/usr/local/lib -levent -ljson cos-cpp-sdk-v5/libs/linux/libcossdk.a -lpthread  -lPocoFoundation -lPocoNet -lPocoNetSSL -lcrypto -lPocoJSON -lstdc++ -o $(TARGET)

clean:
	rm -rf *o $(TARGET)
