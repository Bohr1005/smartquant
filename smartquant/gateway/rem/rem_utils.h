#ifndef SMARTQUANT_REM_UTILS_H
#define SMARTQUANT_REM_UTILS_H
#include <string>
#include "object.h"
#include "utils.h"

namespace SmartQuant
{
    inline std::string to_string(EesEqsIntrumentType chInstrumentType)
    {
        switch (chInstrumentType)
        {
        case EesEqsIntrumentType::EQS_SH_STOCK:
            return "SH_STOCK";
        
        case EesEqsIntrumentType::EQS_SZ_STOCK:
            return "SZ_STOCK";

        case EesEqsIntrumentType::EQS_FUTURE:
            return "FUTURE";
        
        case EesEqsIntrumentType::EQS_FUTURE_OPTION:
            return "FUTURE_OPTION";
        
        case EesEqsIntrumentType::EQS_INDEX_OPTION:
            return "INDEX_OPTION";

        case EesEqsIntrumentType::EQS_STOCK_OPTION:
            return "STOCK_OPTION";

        case EesEqsIntrumentType::EQS_SGE:
            return "SGE";
        
        case EesEqsIntrumentType::EQS_INVALID_TYPE:
            return "INVALID_TYPE";
        
        }
    }

/////////////////////////////////////////////////////////////    

    inline void to_rem(const InstrumentType& type,EesEqsIntrumentType& chInstrumentType)
    {
        switch (type)
        {
        case InstrumentType::FUTURE:
            chInstrumentType = EesEqsIntrumentType::EQS_FUTURE;
            break;
        
        case InstrumentType::FUTURE_OPTION:
            chInstrumentType = EesEqsIntrumentType::EQS_FUTURE_OPTION;
            break;

        case InstrumentType::INDEX_OPTION:
            chInstrumentType = EesEqsIntrumentType::EQS_INDEX_OPTION;
            break;

        case InstrumentType::SGE:
            chInstrumentType = EesEqsIntrumentType::EQS_SGE;
            break;

        case InstrumentType::SH_STOCK:
            chInstrumentType = EesEqsIntrumentType::EQS_SH_STOCK;
            break;
        
        case InstrumentType::SZ_STOCK:
            chInstrumentType = EesEqsIntrumentType::EQS_SZ_STOCK;
            break;

        case InstrumentType::STOCK_OPTION:
            chInstrumentType = EesEqsIntrumentType::EQS_STOCK_OPTION;
            return;
        }
    }
/////////////////////////////////////////////////////////////

    inline DateTime from_rem_time(const char *date, const char *update_time, int millisec = 0)
    {
        static char buffer[21];
        memset(buffer, 0, 21);
        memcpy(buffer, date, 4);
        buffer[4] = '-';
        memcpy(buffer + 5, date + 4, 2);
        buffer[7] = '-';
        memcpy(buffer + 8, date + 6, 2);
        buffer[10] = ' ';
        memcpy(buffer + 11, update_time, 8);
        struct tm time {};
        strptime(buffer, "%Y-%m-%d %H:%M:%S",&time);
        DateTime datetime(time.tm_year+1900,
                          time.tm_mon+1,
                          time.tm_mday,
                          time.tm_hour,
                          time.tm_min,
                          time.tm_sec,
                          millisec);
        return datetime;
    }

/////////////////////////////////////////////////////////////

    inline void from_rem(EESMarketDepthQuoteData* pDepthQuoteData,TickData& tick)
    {
        tick.datetime = from_rem_time(pDepthQuoteData->TradingDay,pDepthQuoteData->UpdateTime,pDepthQuoteData->UpdateMillisec);
        strcpy(tick.symbol,pDepthQuoteData->InstrumentID);
        strcpy(tick.exchange,pDepthQuoteData->ExchangeID);
        tick.gateway = GatewayType::REM;

        tick.last_price = pDepthQuoteData->LastPrice;
        tick.volume = pDepthQuoteData->Volume;
        tick.turnover = pDepthQuoteData->Turnover;

        tick.lower_limit_price = pDepthQuoteData->LowerLimitPrice;
        tick.upper_limit_price = pDepthQuoteData->UpperLimitPrice;

        tick.open_price = pDepthQuoteData->OpenPrice;
        tick.pre_close_price = pDepthQuoteData->PreClosePrice;
        tick.pre_settlement_price = pDepthQuoteData->PreSettlementPrice;
        tick.high_price = pDepthQuoteData->HighestPrice;
        tick.low_price = pDepthQuoteData->LowerLimitPrice;

        tick.ask_1_price = pDepthQuoteData->AskPrice1;
        tick.ask_2_price = pDepthQuoteData->AskPrice2;
        tick.ask_3_price = pDepthQuoteData->AskPrice3;
        tick.ask_4_price = pDepthQuoteData->AskPrice4;
        tick.ask_5_price = pDepthQuoteData->AskPrice5;

        tick.bid_1_price = pDepthQuoteData->BidPrice1;
        tick.bid_2_price = pDepthQuoteData->BidPrice2;
        tick.bid_3_price = pDepthQuoteData->BidPrice3;
        tick.bid_4_price = pDepthQuoteData->BidPrice4;
        tick.bid_5_price = pDepthQuoteData->BidPrice5;

        tick.ask_1_volume = pDepthQuoteData->AskVolume1;
        tick.ask_2_volume = pDepthQuoteData->AskVolume2;
        tick.ask_3_volume = pDepthQuoteData->AskVolume3;
        tick.ask_4_volume = pDepthQuoteData->AskVolume4;
        tick.ask_5_volume = pDepthQuoteData->AskVolume5;

        tick.bid_1_volume = pDepthQuoteData->BidVolume1;
        tick.bid_2_volume = pDepthQuoteData->BidVolume2;
        tick.bid_3_volume = pDepthQuoteData->BidVolume3;
        tick.bid_4_volume = pDepthQuoteData->BidVolume4;
        tick.bid_5_volume = pDepthQuoteData->BidVolume5;
    }
}

#endif