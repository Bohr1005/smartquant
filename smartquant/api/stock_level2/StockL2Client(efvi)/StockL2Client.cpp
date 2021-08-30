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
///ef_vi所需库文件===>>>
#include <poll.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <etherfabric/vi.h>
#include <etherfabric/pd.h>
#include <etherfabric/memreg.h>
#include <netinet/udp.h>
///ef_vi所需库文件===<<<

using namespace std;
StockL2MarketData pTick;
StockL2TradeField pTrade;

struct Packet
{
    uint64_t ef_addr;
    char* pContent;
    int id;
    struct Packet* pNext;
};

enum ef_vi_flags vi_flags = EF_VI_FLAGS_DEFAULT;
struct Packet* pkt_bufs[N_BUFS];
ef_driver_handle dh;
static ef_vi vi;
static void* pMemTotal;
static ef_memreg memreg;
int packetCount = N_BUFS * PKT_BUF_SIZE;
static unsigned rx_posted = 0, rx_completed = 0, refill_flag=0;

struct timespec sleepts, tss1, tss2;

///为efvi增加组播地址及端口过滤
int ef_viAddFilter(const char* strIP, const char* strPort, struct ef_vi* destVitualInterface)
{
    struct addrinfo* pAi;
    struct addrinfo hints;
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = 0;
    hints.ai_protocol = 0;
    hints.ai_addrlen = 0;
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;
    int iResult = getaddrinfo(strIP, strPort, &hints, &pAi);
    printf("getaddrinfo\n");
    if(iResult != 0)
        return -1;

    ef_filter_spec filter_spec;
    ef_filter_spec_init(&filter_spec, EF_FILTER_FLAG_NONE);
    struct sockaddr_in *lsin = (struct sockaddr_in *)(pAi->ai_addr);
    iResult = ef_filter_spec_set_ip4_local(&filter_spec, IPPROTO_UDP, lsin->sin_addr.s_addr, lsin->sin_port);
    printf("ef_filter_spec_set_ip4_local\n");
    if(iResult != 0)
        return -1;

    iResult = ef_vi_filter_add(destVitualInterface, dh, &filter_spec, NULL);
    printf("ef_vi_filter_add\n");
    if(iResult < 0)
        return -1;
	printf("ef_viAddFilter %s is OK!\n", strIP);
}

///填充所有rx_buf
void rx_post_all()
{
	struct Packet* pkt_buf;
	for(int i=0; i<N_BUFS; ++i)
	{
		pkt_buf =pkt_bufs[i];
		ef_vi_receive_init(&vi, pkt_buf->ef_addr + RX_DMA_OFF, pkt_buf->id);
	}
	ef_vi_receive_push(&vi);
}

void rx_post()
{
	struct Packet* pkt_buf;
	unsigned buf_i;
	for(int i=0; i<N_BUFS/2; ++i)
	{
		buf_i = rx_posted % N_BUFS;
		pkt_buf = pkt_bufs[buf_i];
		ef_vi_receive_init(&vi, pkt_buf->ef_addr + RX_DMA_OFF, pkt_buf->id);
		++rx_posted;
	}
	ef_vi_receive_push(&vi);
}

//重新填充buf
void refill_rx(int cpuid)
{
	//绑定CPU
	//cpu_set_t cpu_set;
    //CPU_ZERO(&cpu_set);
    //CPU_SET(cpuid, &cpu_set);
	//sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set);
	while(1)
	{
		if(refill_flag == 1) 
		{
			//printf("rx_completed=%d\n", rx_completed);
			rx_post(); refill_flag=0;
		}
		nanosleep(&sleepts, NULL);
	}
}

//usage："./StockL2Client enp5s0f0 1"
//StockL2Client 可执行文件名
//enp5s0f0 接收组播的网口名
//1 绑定的cpuid
int main(int argc, char **argv)
{
	//加入组播 START========================================================>>
	//printf("加入组播...\n");
	struct sockaddr_in addr;
	int fd, nbytes, addrlen;
	struct ip_mreq mreq;

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
	printf("加入组播224.1.1.100成功!\n");
	//加入组播 END==========================================================//

	//EFVI START============================================================>>
	//printf("启动EFVI...\n");
    int iResult = 0;
    iResult = ef_driver_open(&dh);
    printf("ef_driver_open\n");
    if(iResult < 0)
        return -1;

    struct ef_pd pd;
    iResult = ef_pd_alloc_by_name(&pd, dh, argv[1], EF_PD_DEFAULT);//argv[1]为本地接收组播的网卡名称
    printf("ef_pd_alloc_by_name(%s)\n", argv[1]);
    if(iResult < 0)
        return -1;

    iResult = ef_vi_alloc_from_pd(&vi, dh, &pd, dh, -1, N_BUFS, 0, NULL, -1, vi_flags);
    printf("ef_vi_alloc_from_pd\n");
    if(iResult < 0)
        return -1;

    iResult = posix_memalign(&pMemTotal, PAGE_SIZE, packetCount);
    printf("posix_memalign(%d)\n", packetCount);
    if(iResult != 0)
        return -1;

    iResult = ef_memreg_alloc(&memreg, dh, &pd, dh, pMemTotal, packetCount);
    printf("ef_memreg_alloc\n");
    if(iResult < 0)
        return -1;

	printf("txq_size=%d\n", ef_vi_transmit_capacity(&vi));
	printf("rxq_size=%d\n", ef_vi_receive_capacity(&vi));
	printf("evq_size=%d\n", ef_eventq_capacity(&vi));
	printf("sync_check_enabled=%d\n", (vi.vi_out_flags & EF_VI_OUT_CLOCK_SYNC_STATUS) != 0);

	struct Packet* pkt_buf;
    for(int i=0; i<N_BUFS; ++i)
    {
        pkt_buf = (struct Packet*)((char*)pMemTotal + (size_t)i*PKT_BUF_SIZE);
        pkt_buf->pContent = (char*)pkt_buf + RX_DMA_OFF;
        pkt_buf->id = i;
		pkt_buf->ef_addr = ef_memreg_dma_addr(&memreg, i * PKT_BUF_SIZE);
        pkt_bufs[i] = pkt_buf;
    }

	ef_viAddFilter("224.1.1.100", "28200", &vi);//证券行情数据

	printf("EFVI启动成功!\n");

	//填充所有rx_buf
	rx_post_all();

	//启动异步填充线程
	sleepts.tv_sec = 0;
	sleepts.tv_nsec = 1;
	thread t = thread(refill_rx, NULL);
	t.detach();

	//主线程绑定到指定的CPU核心
	cpu_set_t cpu_set;
	CPU_ZERO(&cpu_set);
	CPU_SET(atoi(argv[2]), &cpu_set);
	sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set);

	ef_event eventList[RX_POST_SIZE];//定义事件队列
	int nEventCount = 0;

	///efvi模式接收///
	while(1)
    {	
		nEventCount = ef_eventq_poll(&vi, eventList, sizeof(eventList) / sizeof(eventList[0]));//轮询efvi事件
		if (nEventCount > 0)
		{
			for(int i = 0;i < nEventCount; ++i)
			{
				struct DataType* dt = (DataType*)(pkt_bufs[eventList[i].rx.rq_id]->pContent + UDP_HEADER_LEN);
				if (dt->type == 9001)//证券行情数据
				{
					memcpy(&pTick, pkt_bufs[eventList[i].rx.rq_id]->pContent + UDP_HEADER_LEN + sizeof(DataType), sizeof(StockL2MarketData));//将字节转为结构体
					printf("%s,%06d,%06d,%03d,[%.4f,%d],[%.4f,%d],[%.4f,%d],[%.4f,%d],[%.4f,%d],[%.4f,%d],[%.4f,%d],[%.4f,%d],[%.4f,%d],[%.4f,%d]\n",
						pTick.SecurityID,
						pTick.TradingDay,
						pTick.UpdateTime,
						pTick.UpdateMillisec,
						pTick.BidPrice5,
						pTick.BidVolume5,
						pTick.BidPrice4,
						pTick.BidVolume4,
						pTick.BidPrice3,
						pTick.BidVolume3,
						pTick.BidPrice2,
						pTick.BidVolume2,
						pTick.BidPrice1,
						pTick.BidVolume1,
						pTick.AskPrice1,
						pTick.AskVolume1,
						pTick.AskPrice2,
						pTick.AskVolume2,
						pTick.AskPrice3,
						pTick.AskVolume3,
						pTick.AskPrice4,
						pTick.AskVolume4,
						pTick.AskPrice5,
						pTick.AskVolume5);
				}

				if (dt->type == 9002)//证券成交数据
				{
					memcpy(&pTrade, pkt_bufs[eventList[i].rx.rq_id]->pContent + UDP_HEADER_LEN + sizeof(DataType), sizeof(StockL2TradeField));//将字节转为结构体
					printf("%s,%06d,%03d,%.4f,%d\n",
						pTrade.SecurityID,
						pTrade.UpdateTime,
						pTrade.UpdateMillisec,
						pTrade.LastPrice,
						pTrade.CurrVolume);
				}
				++rx_completed;	if(rx_completed % (N_BUFS/2) == 0) refill_flag=1;
			}
		}
	}
	//EFVI END================================================================//
}
