#pragma once
#include <stdint.h>
#include <vector>

typedef int8_t				Int8;
typedef int16_t				Int16;
typedef int32_t				Int32;
typedef int64_t				Int64;
typedef uint8_t				uInt8;
typedef uint16_t			uInt16;
typedef uint32_t			uInt32;
typedef uint64_t			uInt64;

//#pragma pack(push, 8)
struct DataType
{
	int type;						//0	行情数据；1 成交数据
};

///深度行情
struct StockL2MarketData
{
    char SecurityID[32];
    int StockType;
    unsigned int UpdateTime;        //1 市场时间 Uint 形如093000，HHMMSS，精确到秒，第61字段表示毫秒部分ms
    double LastPrice;               //2 现价 Double 不同品种小数位不同
    double PreClosePrice;           //3 前收价 Double 同2
    double OpenPrice;               //4 开盘价 Double 同2
    double HighestPrice;            //5 最高价 Double 同2
    double LowestPrice;             //6 最低价 Double 同2
    long long Volume;               //7 成交总量 Int64 
    double Turnover;                //8 成交总额 Double 
    int CurrVolume;                 //9 成交现手 int 
    double CurrAmount;              //10 成交现额 Double 
    double BidPrice1;               //11 买一价 Double 同2
    double BidPrice2;               //12 买二价 Double 同2
    double BidPrice3;               //13 买三价 Double 同2
    double BidPrice4;               //14 买四价 Double 同2
    double BidPrice5;               //15 买五价 Double 同2
    double BidPrice6;               //16 买六价 Double 同2
    double BidPrice7;               //17 买七价 Double 同2
    double BidPrice8;               //18 买八价 Double 同2
    double BidPrice9;               //19 买九价 Double 同2
    double BidPrice10;              //20 买十价 Double 同2
    double AskPrice1;               //21 卖一价 Double 同2
    double AskPrice2;               //22 卖二价 Double 同2
    double AskPrice3;               //23 卖三价 Double 同2
    double AskPrice4;               //24 卖四价 Double 同2
    double AskPrice5;               //25 卖五价 Double 同2
    double AskPrice6;               //26 卖六价 Double 同2
    double AskPrice7;               //27 卖七价 Double 同2
    double AskPrice8;               //28 卖八价 Double 同2
    double AskPrice9;               //29 卖九价 Double 同2
    double AskPrice10;              //30 卖十价 Double 同2
    int BidVolume1;                 //31 买一量 Int 
    int BidVolume2;                 //32 买二量 Int 
    int BidVolume3;                 //33 买三量 Int 
    int BidVolume4;                 //34 买四量 Int 
    int BidVolume5;                 //35 买五量 Int 
    int BidVolume6;                 //36 买六量 Int 
    int BidVolume7;                 //37 买七量 Int 
    int BidVolume8;                 //38 买八量 Int 
    int BidVolume9;                 //39 买九量 Int 
    int BidVolume10;                //40 买十量 Int 
    int AskVolume1;                 //41 卖一量 Int 
    int AskVolume2;                 //42 卖二量 Int 
    int AskVolume3;                 //43 卖三量 Int 
    int AskVolume4;                 //44 卖四量 Int 
    int AskVolume5;                 //45 卖五量 Int 
    int AskVolume6;                 //46 卖六量 Int 
    int AskVolume7;                 //47 卖七量 Int 
    int AskVolume8;                 //48 卖八量 Int 
    int AskVolume9;                 //49 卖九量 Int 
    int AskVolume10;                //50 卖十量 Int 
    double BidVWAP;                 //51 委买加权平均价格 Double Bid Volume Weighted Average Price
    long long BidVolumeTotal;       //52 委买总量 Int64 
    double ASKVWAP;                 //53 委卖加权平均价格 Double Ask Volume Weighted Average Price
    long long AskVolumeTotal;       //54 委卖总量 Int64 
    int TradeVolumeTotal;           //55 总成交笔数 Int 
    double f56;                     //56 （盘前）虚拟价格 Double 
    long long i64_57;               //57 （盘前）虚拟匹配量 Int64 
    int i64_58;                     //58 （盘前）买方虚拟匹配量 Int64 
    int i64_59;                     //59 （盘前）卖方虚拟匹配量 Int64 
    int TradingDay;                 //60 市场日期 Int 形如20140906，YYYYMMDD
    int UpdateMillisec;             //61 高精度时间 Int 毫秒数(0-999)
    double UpperLimitPrice;         //63 涨停价 Double 
    double LowerLimitPrice;         //64 跌停价 Double 
    double AveragePrice;            //65 加权平均价 Double 
    int MarketStatus;    //67 市场状态 Int 见交易状态表
    double IOPV;                    //80 IOPV Double 
    double SettlementPrice;         //100 结算价 Double 
    double PreSettlementPrice;      //101 昨日结算价 Double 
    int int102;                     //102 升跌1 Int 
    double f103;                    //103 动态参考价 Double 
    char char104[8];                //104 品种名称 String 
    int int105;                     //105 到期收益率 Int 
    char char106[8];                //106 证券信息前缀 String 
    int int107;                     //107 市盈率1 Int 
    int int108;                     //108 市盈率2 Int 
    int int109;                     //109 升跌2 Int 对比上一笔
    int Direction;                  //110 买卖方向 Int 逐笔成交、逐笔委托、委托队列使用，’B’-买（bid），’S’-卖（ask），’G’-借入，’F’-出借， 空格 - 未知/不适用
    double f111;                    //111 委托价格 double 逐笔委托使用
    int int112;                     //112 订单数量 Int 逐笔委托使用
    int TradeID;                    //113 成交编号 Int 逐笔成交序号
    int int114;                     //114 委托号 Int 逐笔委托用
    int int115;                     //115 委托类型 Int 见逐笔委托类型表
    int int116;                     //116 交易所频道号 Int 逐笔成交/委托使用
    int OrderIDOfSeller;            //117 卖方委托序号 Int 逐笔成交用
    int OrderIDOfBuyer;             //118 买方委托序号 Int 逐笔成交用
    int TradeType;                  //119 成交类型 Int 见逐笔成交类型表
    int PreOpenInterest_f;          //120 昨持仓量 Int 期货
    int OpenInterest_f;             //121 持仓量 Int 期货
    double AvgPrice_f;              //122 当日均价 Double 期货
    int ActionDay_f;                //123 业务日期 Int 期货
    int int124;                     //124 明细个数 Int 委托队列中配合订单明细使用
    int int125;                     //125 溢价率 Int 权证
    int int126;                     //126 加权平均涨跌BP Int 质押式回购
    int int127;                     //127 昨收成交量加权平均利率 Int 质押式回购
    int int128;                     //128 基金T-1日净值 Int 
    int int129;                     //129 基金实时参考净值 Int 
};

struct StockL2TradeField
{
    char SecurityID[32];
    int StockType;
    int UpdateTime;					//1 市场时间 Uint 形如093000，HHMMSS，精确到秒，第61字段表示毫秒部分ms
    int UpdateMillisec;             //61 高精度时间 Int 毫秒数(0-999)
    double LastPrice;               //2 现价 Double 不同品种小数位不同
    int CurrVolume;                 //9 成交现手 int 
    int TradeID;                    //113 成交编号 Int 逐笔成交序号
    int Direction;                  //110 买卖方向 Int 逐笔成交、逐笔委托、委托队列使用，’B’-买（bid），’S’-卖（ask），’G’-借入，’F’-出借， 空格 - 未知/不适用
    int OrderIDOfSeller;            //117 卖方委托序号 Int 逐笔成交用
    int OrderIDOfBuyer;             //118 买方委托序号 Int 逐笔成交用
    int ExchangeChannel;            //116 交易所频道号 Int 逐笔成交/委托使用
    int TradeType;					//119 成交类型 Int 见逐笔成交类型表
};
//#pragma pack(pop)

///ef_vi启动参数====>>>
#define ROUND_UP(p, align) (((p)+(align)-1u) & ~((align)-1u))
#define PKT_BUF_SIZE 4096
#define RX_DMA_OFF ROUND_UP(sizeof(struct Packet), EF_VI_DMA_ALIGN)
#define PAGE_SIZE 4096
#define UDP_HEADER_LEN 42
#define N_BUFS 4096
#define RX_POST_SIZE 32
///ef_vi启动参数====<<<