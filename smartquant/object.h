#ifndef SMARTQUANT_OBJECT_H
#define SMARTQUANT_OBJECT_H
#include "declare.h"
#include <string>
#include "utils.h"

SMARTQUANT_NAMESPACE_START

/////////////////////////////////////////////////////////////

const int INSTRUMENT_ID_LEN = 32;
const int EXCHANGE_ID_LEN = 16;

/////////////////////////////////////////////////////////////

enum class InstrumentType : uint32_t
{
    SH_STOCK = 0,
    SZ_STOCK = 1,
    FUTURE = 2,
    FUTURE_OPTION = 3,
    INDEX_OPTION = 4,
    STOCK_OPTION = 5,
    SGE = 6
};

enum class Exchange : uint32_t
{
    CFFEX = 0,
    SHFE = 1,
    CZCE = 2,
    DCE = 3,
    INE = 4,
    SH = 5,
    SZ = 6,
    SGE = 7
};

enum class Offset : uint32_t
{
    OPEN = 0,
    CLOSE = 1,
    CLOSETODAY = 2,
    CLOSEYESTERDAY = 3,
    AUTO = 4
};


enum class Status : uint32_t
{
    SUBMITTED = 0,
    PENDING = 1,
    CANCELLED = 2,
    REJECT = 3,
    FILLED = 4,
    PARTIALFILLEDACTIVE = 5,
    PARTIALFILLEDNOTACTIVE = 6,
    ERROR  = 7,
    UNKOWN = 8
};


enum class OrderType : uint32_t
{
    LIMIT = 0,
    MARKET = 1,
    FAK = 2,
    FOK = 3
};


enum class OrderSide : uint32_t
{
    LONG = 0,
    SHORT = 1
};

/////////////////////////////////////////////////////////////

enum class GatewayType : uint32_t
{
    UNKNOW,
    CTP,
    L2,
    REM
};


enum class Frequency : uint32_t
{
    MINUTE = 0,
    HOUR = 1
};


struct BaseData
{
    char symbol[INSTRUMENT_ID_LEN];
    char exchange[EXCHANGE_ID_LEN];
    GatewayType gateway;
};


struct TickData : public BaseData
{
    DateTime datetime;

    double pre_close_price;
    double pre_settlement_price;

    double last_price;
    int64_t volume;
    double turnover;

    double open_price;
    double high_price;
    double low_price;

    double upper_limit_price;
    double lower_limit_price;

    double bid_1_price;
    double bid_2_price;
    double bid_3_price;
    double bid_4_price;
    double bid_5_price;

    double ask_1_price;
    double ask_2_price;
    double ask_3_price;
    double ask_4_price;
    double ask_5_price;

    double bid_1_volume;
    double bid_2_volume;
    double bid_3_volume;
    double bid_4_volume;
    double bid_5_volume;

    double ask_1_volume;
    double ask_2_volume;
    double ask_3_volume;
    double ask_4_volume;
    double ask_5_volume;
};


struct BarData : public BaseData
{
    DateTime datetime;

    int64_t volume;
    double open_price;
    double high_price;
    double low_price;
    double close_price;

    Frequency frequency;
    int window;
};


struct OrderData : public BaseData
{
    int64_t order_id;

    OrderType type;
    OrderSide side;
    Offset offset;
    Status status;

    double price;
    int volume;
    int volume_traded; 
    int volume_left;
};



struct TradeData : public BaseData
{
    int64_t trade_id;
    int64_t order_id; 

    OrderSide side;
    Offset offset;

    double price;
    int volume;
};


/////////////////////////////////////////////////////////////

struct OrderRequest : public BaseData
{
    OrderType type;
    OrderSide side;
    Offset offset;

    double price;
    int volume;
};


struct CancelRequest : public BaseData
{
    int64_t order_ref;
};


SMARTQUANT_NAMESPACE_END

#endif