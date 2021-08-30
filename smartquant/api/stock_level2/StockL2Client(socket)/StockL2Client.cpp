#include "dataDefine.h"
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <thread>
#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <unordered_map>
#include "utils.h"
#include "object.h"
#include <zmq.hpp>
#include <zmq_addon.hpp>

using namespace std;

StockL2MarketData pTick;
#define MSGBUFSIZE 4096
struct timespec sleepts, tss1, tss2;


class Index
{
public:
	Index(SmartQuant::Config& config)
	{
		auto children = config.children();
		for (auto& child : children)
		{
			weight_[child.name()] = child.get_double("weight");
		}
	}

	void update(char* symbol,double price)
	{
		if (weight_.find(symbol)!=weight_.end())
		{
			price_[symbol] = price;
			if (!ready_ && weight_.size()==price_.size())
			{
				ready_ = true;
			}
		}
	}

	double last_price()
	{
		if(ready_)
		{
			double price = 0.0;
			for (auto& e : weight_)
			{
				price += (price_[e.first] * e.second);
			}
			return price;
		}
		else
		{
			return 0.0;
		}
	}

private:
	std::unordered_map<std::string,double> weight_;

	std::unordered_map<std::string,double> price_;

	bool ready_;
};



int main(int argc, char **argv)
{
	//加入组播 START========================================================>>
	//printf("加入组播...\n");
	struct sockaddr_in addr;
	int fd, nbytes, addrlen;
	struct ip_mreq mreq;
	char msgbuf[MSGBUFSIZE];

	std::string path = "/home/bohr/smartquant/smartquant/strategy/stock_share.json";
	SmartQuant::Config config(path);
	Index hs300(config);
	Index zz500(config);

	u_int yes=1; // MODIFICATION TO ORIGINAL

	// create what looks like an ordinary UDP socket
	if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		printf("socket failed\n");
		return -1;
	}

	// allow multiple sockets to use the same PORT number
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
	{
		printf("Reusing ADDR failed\n");
		return -1;
	}

	// set up destination address
	memset(&addr,0,sizeof(addr));
	//addr.sin_family=AF_INET;
	//addr.sin_addr.s_addr=htonl(INADDR_ANY); // N.B.: differs from sender
	addr.sin_port=htons(28200);//修改为要接收的组播端口

	// bind to receive address
	if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		printf("bind failed\n");
		return -1;
	}

	// use setsockopt() to request that the kernel join a multicast group
	mreq.imr_multiaddr.s_addr=inet_addr("224.1.1.100");//修改为要加入的组播地址
	mreq.imr_interface.s_addr=htonl(INADDR_ANY);//本地随机地址
	//mreq.imr_interface.s_addr=inet_addr("172.18.5.8");//本机指定地址
	if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		printf("setsockopt failed\n");
		return -1;
	}
	//加入组播 END==========================================================//

	// 循环接受用户输入的消息发送组播消息 //
	while (true) 
	{
		addrlen = sizeof(addr);
		if ((nbytes = recvfrom(fd, msgbuf, MSGBUFSIZE, 0, (struct sockaddr *) &addr, (socklen_t*)&addrlen)) > 0)
		{
			struct DataType* dt = (DataType*)(msgbuf);
			if (dt->type == 9001)//证券行情数据
			{
				memcpy(&pTick, msgbuf + sizeof(DataType), sizeof(StockL2MarketData));//将字节转为结构体
			}
			hs300.update(pTick.SecurityID,pTick.LastPrice);
			zz500.update(pTick.SecurityID,pTick.LastPrice);
		}
	}
}
