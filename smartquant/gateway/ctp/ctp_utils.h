#ifndef SMARTQUANT_CTP_UTILS_H
#define SMARTQUANT_CTP_UTILS_H
#include <string>
#include "api/ctp/ThostFtdcUserApiStruct.h"
#include "object.h"
#include "utils.h"

namespace SmartQuant
{
    inline std::string disconnected_reason(int reason)
    {
        switch (reason)
        {
        case 0x1001:
            return "网络读失败";
        case 0x1002:
            return "网络写失败";
        case 0x2001:
            return "接收心跳超时";
        case 0x2002:
            return "发送心跳失败";
        case 0x2003:
            return "收到错误报文";
        default:
            return "Unknown";
        }
    }

/////////////////////////////////////////////////////////////

    inline void from_ctp_direction(TThostFtdcDirectionType &ctp_direction,OrderSide& side)
    {
        if (ctp_direction == THOST_FTDC_D_Buy)
        {
            side = OrderSide::LONG;
            return;
        }
        else
        {
            side = OrderSide::SHORT;
            return;
        }
    }

    inline void to_ctp_direction(TThostFtdcDirectionType &ctp_direction,const OrderSide& side)
    {
        if (side == OrderSide::LONG) 
        {
            ctp_direction = THOST_FTDC_D_Buy;
            return;
        }
        else
        {
            ctp_direction = THOST_FTDC_D_Sell;
            return;
        }
    }

    inline std::string ctp_direction(TThostFtdcDirectionType &ctp_direction)
    {
        if (ctp_direction == THOST_FTDC_D_Buy)
        {
            return "LONG";
        }
        else
        {
            return "SHORT";
        }
    }
/////////////////////////////////////////////////////////////

    // ctp_comb_offset for order
    inline void from_ctp_comb_offset(TThostFtdcCombOffsetFlagType ctp_offset, Offset &offset)
    {
        if (ctp_offset[0] == THOST_FTDC_OF_Close)
        {
            offset = Offset::CLOSE;
            return;
        }
        else if (ctp_offset[0] == THOST_FTDC_OF_Open)
        {    
            offset = Offset::OPEN;
            return;
        }
        else if (ctp_offset[0] == THOST_FTDC_OF_CloseToday)
        {
            offset = Offset::CLOSETODAY;
            return;
        }
        else
        {
            offset = Offset::CLOSEYESTERDAY;
            return;
        }
    }

    inline void to_ctp_comb_offset(TThostFtdcCombOffsetFlagType ctp_offset, const Offset &offset)
    {
        if (offset == Offset::CLOSE)
        {
            ctp_offset[0] = THOST_FTDC_OF_Close;
            return;
        }
        else if (offset == Offset::OPEN)
        {
            ctp_offset[0] = THOST_FTDC_OF_Open;
            return;
        }
        else if (offset == Offset::CLOSETODAY)
        {
            ctp_offset[0] = THOST_FTDC_OF_CloseToday;
            return;
        }
        else
        {
            ctp_offset[0] = THOST_FTDC_OF_CloseYesterday;
            return;
        }
    }

    // ctp_offset for trade
    inline void from_ctp_offset(const TThostFtdcOffsetFlagType ctp_offset, Offset &offset)
    {
        if (ctp_offset == THOST_FTDC_OF_Close)
        {
            offset = Offset::CLOSE;
            return;
        }
        else if (ctp_offset == THOST_FTDC_OF_Open)
        {
            offset = Offset::OPEN;
            return;
        }
        else if (ctp_offset == THOST_FTDC_OF_CloseToday)
        {
            offset = Offset::CLOSETODAY;
            return;
        }
        else 
        {
            offset = Offset::CLOSEYESTERDAY;
            return;
        }
    }

    inline std::string ctp_comb_offset(const TThostFtdcCombOffsetFlagType ctp_offset)
    {   
        if (ctp_offset[0] == THOST_FTDC_OF_Close)
        {
            return "CLOSE";
        }
        else if (ctp_offset[0] == THOST_FTDC_OF_Open)
        {
            return "OPEN";
        }
        else if (ctp_offset[0] == THOST_FTDC_OF_CloseToday)
        {
            return "CLOSETODAY";
        }
        else
        {
            return "CLOSEYESTERDAY";
        }
    }

/////////////////////////////////////////////////////////////

    inline void from_ctp_price_type(TThostFtdcOrderPriceTypeType &price_type, TThostFtdcVolumeConditionType &volume_condition, TThostFtdcTimeConditionType &time_condition, OrderType &type)
    {
        if (price_type == THOST_FTDC_OPT_LimitPrice)
        {
            if (time_condition == THOST_FTDC_TC_GFD)
            {
                type = OrderType::LIMIT;
            }
            else if(time_condition == THOST_FTDC_TC_IOC)
            {
                if (volume_condition == THOST_FTDC_VC_AV)
                {
                    type = OrderType::FAK;
                } 
                else
                {
                    type = OrderType::FOK;
                }
            }
        } 
        else if (price_type == THOST_FTDC_OPT_AnyPrice)
        {
            type = OrderType::MARKET;
        }
    }

    inline void to_ctp_price_type(TThostFtdcOrderPriceTypeType &price_type, TThostFtdcVolumeConditionType &volume_condition, TThostFtdcTimeConditionType &time_condition, const OrderType &type)
    {
        if (type == OrderType::LIMIT)
        {
            price_type = THOST_FTDC_OPT_LimitPrice;
            volume_condition = THOST_FTDC_VC_AV;
            time_condition = THOST_FTDC_TC_GFD;
            return;
        }
        else if (type == OrderType::MARKET)
        {
            price_type = THOST_FTDC_OPT_AnyPrice;
            volume_condition = THOST_FTDC_VC_AV;
            time_condition = THOST_FTDC_TC_IOC;
            return;
        }
        else if (type == OrderType::FAK)
        {
            price_type = THOST_FTDC_OPT_LimitPrice;
            volume_condition = THOST_FTDC_VC_AV;
            time_condition = THOST_FTDC_TC_IOC;
            return;
        }
        else 
        {
            price_type = THOST_FTDC_OPT_LimitPrice;
            volume_condition = THOST_FTDC_VC_CV;
            time_condition = THOST_FTDC_TC_IOC;
        }   return;
    }

/////////////////////////////////////////////////////////////

    inline void from_ctp_order_status(const TThostFtdcOrderStatusType &order_status,const TThostFtdcOrderSubmitStatusType &submit_status,Status &status)
    {
        if (order_status == THOST_FTDC_OST_AllTraded) // 0 全部成交
        {
            status = Status::FILLED;
        }
        else if (order_status == THOST_FTDC_OST_PartTradedQueueing) // 1 部分成交，订单还在交易所撮合队列中
        {
            status = Status::PARTIALFILLEDACTIVE;
        }
        else if (order_status == THOST_FTDC_OST_PartTradedNotQueueing) // 2 部分成交，订单不再交易所撮合队列中
        {
            status = Status::PARTIALFILLEDNOTACTIVE;
        }
        else if (order_status == THOST_FTDC_OST_NoTradeQueueing) // 3 未成交，订单还在交易所撮合队列中
        {
            status = Status::PENDING;
        }
        else if (order_status == THOST_FTDC_OST_Canceled) // 5 已经撤单
        {
            status = Status::CANCELLED;
        }
        else // 未知
        {
            if (submit_status == THOST_FTDC_OSS_InsertSubmitted)
            {
                status = Status::SUBMITTED;
            } 
            else if (submit_status == THOST_FTDC_OSS_Accepted)
            {
                status = Status::PENDING;
            } 
            else if (submit_status == THOST_FTDC_OSS_InsertRejected)
            {
                status = Status::ERROR;
            }
            return;
        }
    }
    inline std::string ctp_status(const TThostFtdcOrderStatusType &order_status,const TThostFtdcOrderSubmitStatusType &submit_status)
    {

        if (order_status == THOST_FTDC_OST_AllTraded)
        {
            return "FILLED";
        }
        else if (order_status == THOST_FTDC_OST_PartTradedQueueing)
        {
            return "PARTIALFILLEDACTIVE";
        }
        else if (order_status == THOST_FTDC_OST_PartTradedNotQueueing)
        {
            return "PARTIALFILLEDNOTACTIVE";
        }
        else if (order_status == THOST_FTDC_OST_NoTradeQueueing)
        {
            return "PENDING";
        }
        else if (order_status == THOST_FTDC_OST_Canceled) 
        {
            return "CANCELLED";
        }
        else
        {
            if (submit_status == THOST_FTDC_OSS_InsertSubmitted)
            {
                return "SUBMITTED";
            } else if (submit_status == THOST_FTDC_OSS_Accepted)
            {
                return "PENDING";
            } else if (submit_status == THOST_FTDC_OSS_InsertRejected)
            {
                return "Status::ERROR";
            }
        }   
    }
/////////////////////////////////////////////////////////////

    inline DateTime from_ctp_time(const char *date, const char *update_time, int millisec = 0)
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

    inline void from_ctp(CThostFtdcDepthMarketDataField *pDepthMarketData,TickData& tick)
    {
        tick.datetime = from_ctp_time(pDepthMarketData->ActionDay,pDepthMarketData->UpdateTime,pDepthMarketData->UpdateMillisec);
        strcpy(tick.symbol,pDepthMarketData->InstrumentID);
        strcpy(tick.exchange,pDepthMarketData->ExchangeID);
        tick.gateway = GatewayType::CTP;

        tick.last_price = pDepthMarketData->LastPrice;
        tick.volume = pDepthMarketData->Volume;
        tick.turnover = pDepthMarketData->Turnover;

        tick.lower_limit_price = pDepthMarketData->LowerLimitPrice;
        tick.upper_limit_price = pDepthMarketData->UpperLimitPrice;

        tick.open_price = pDepthMarketData->OpenPrice;
        tick.pre_close_price = pDepthMarketData->PreClosePrice;
        tick.pre_settlement_price = pDepthMarketData->PreSettlementPrice;
        tick.high_price = pDepthMarketData->HighestPrice;
        tick.low_price = pDepthMarketData->LowestPrice;

        tick.ask_1_price = pDepthMarketData->AskPrice1;
        tick.ask_2_price = pDepthMarketData->AskPrice2;
        tick.ask_3_price = pDepthMarketData->AskPrice3;
        tick.ask_4_price = pDepthMarketData->AskPrice4;
        tick.ask_5_price = pDepthMarketData->AskPrice5;

        tick.bid_1_price = pDepthMarketData->BidPrice1;
        tick.bid_2_price = pDepthMarketData->BidPrice2;
        tick.bid_3_price = pDepthMarketData->BidPrice3;
        tick.bid_4_price = pDepthMarketData->BidPrice4;
        tick.bid_5_price = pDepthMarketData->BidPrice5;

        tick.ask_1_volume = pDepthMarketData->AskVolume1;
        tick.ask_2_volume = pDepthMarketData->AskVolume2;
        tick.ask_3_volume = pDepthMarketData->AskVolume3;
        tick.ask_4_volume = pDepthMarketData->AskVolume4;
        tick.ask_5_volume = pDepthMarketData->AskVolume5;

        tick.bid_1_volume = pDepthMarketData->BidVolume1;
        tick.bid_2_volume = pDepthMarketData->BidVolume2;
        tick.bid_3_volume = pDepthMarketData->BidVolume3;
        tick.bid_4_volume = pDepthMarketData->BidVolume4;
        tick.bid_5_volume = pDepthMarketData->BidVolume5;
    }

/////////////////////////////////////////////////////////////

    inline void from_ctp(CThostFtdcOrderField *pOrder,OrderData& order)
    {
        strcpy(order.symbol,pOrder->InstrumentID);
        strcpy(order.exchange,pOrder->ExchangeID);
        from_ctp_comb_offset(pOrder->CombOffsetFlag,order.offset);
        from_ctp_order_status(pOrder->OrderStatus,pOrder->OrderSubmitStatus,order.status);
        from_ctp_direction(pOrder->Direction,order.side);
        from_ctp_price_type(pOrder->OrderPriceType,pOrder->VolumeCondition,pOrder->TimeCondition,order.type);
        order.gateway = GatewayType::CTP;
        order.price = pOrder->LimitPrice;
        order.order_id = atoi(pOrder->OrderRef);
        order.volume = pOrder->VolumeTotalOriginal;
        order.volume_traded = pOrder->VolumeTraded;
        order.volume_left = pOrder->VolumeTotal;
    }
/////////////////////////////////////////////////////////////

    inline void from_ctp(CThostFtdcInputOrderField *pInputOrder,OrderData& order)
    {
        strcpy(order.symbol,pInputOrder->InstrumentID);
        strcpy(order.exchange,pInputOrder->ExchangeID); 
        order.status = Status::ERROR;
        from_ctp_comb_offset(pInputOrder->CombOffsetFlag,order.offset);
        from_ctp_direction(pInputOrder->Direction,order.side);
        from_ctp_price_type(pInputOrder->OrderPriceType,pInputOrder->VolumeCondition,pInputOrder->TimeCondition,order.type);
        order.gateway = GatewayType::CTP;
        order.price = pInputOrder->LimitPrice;
        order.order_id = atoi(pInputOrder->OrderRef);
    }

    inline void from_ctp(CThostFtdcInputOrderActionField *pInputOrder,OrderData& order)
    {
        strcpy(order.symbol,pInputOrder->InstrumentID);
        strcpy(order.exchange,pInputOrder->ExchangeID); 
        order.status = Status::ERROR;
        order.gateway = GatewayType::CTP;
        order.price = pInputOrder->LimitPrice;
        order.order_id = atoi(pInputOrder->OrderRef);
    }  
/////////////////////////////////////////////////////////////

    inline void from_ctp(CThostFtdcTradeField *pTrade,TradeData& trade)
    {
        strcpy(trade.symbol,pTrade->InstrumentID);
        strcpy(trade.exchange,pTrade->ExchangeID);
        trade.gateway = GatewayType::CTP;
        trade.price = pTrade->Price;
        trade.volume = pTrade->Volume;
        trade.order_id = atoi(pTrade->OrderRef);
        trade.trade_id = atoi(pTrade->TradeID);
        from_ctp_offset(pTrade->OffsetFlag,trade.offset);
        from_ctp_direction(pTrade->Direction,trade.side);
    }

/////////////////////////////////////////////////////////////

    inline void to_ctp(CThostFtdcInputOrderField *pOrder,const OrderRequest& req)
    {
        strcpy(pOrder->InstrumentID,req.symbol);
        strcpy(pOrder->ExchangeID,req.exchange);
        to_ctp_direction(pOrder->Direction,req.side);
        to_ctp_comb_offset(pOrder->CombOffsetFlag,req.offset);
        pOrder->CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
        pOrder->VolumeTotalOriginal = req.volume;
        pOrder->ContingentCondition = THOST_FTDC_CC_Immediately;
        to_ctp_price_type(pOrder->OrderPriceType,pOrder->VolumeCondition,pOrder->TimeCondition,req.type);
        pOrder->MinVolume = 1;
        pOrder->ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
        pOrder->IsAutoSuspend = 0;
        pOrder->UserForceClose = 0;
        pOrder->LimitPrice = req.price;
    }
} // namespace SmartQuant

#endif
        