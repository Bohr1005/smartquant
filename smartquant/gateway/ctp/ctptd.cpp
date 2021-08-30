#include "gateway/ctp/ctp_gateway.h"
#include "gateway/ctp/ctp_utils.h"
#include "object.h"
#include <ctime>
#include <chrono>

SMARTQUANT_NAMESPACE_START

CtpTd::CtpTd(CtpGateway* gateway) : gateway_(gateway),api_(nullptr)
{
    auto config = gateway_->trd_config();
    auto log_path = config.get_string("log_path");
    auto level = config.get_string("log_level");
    auto format = config.get_string("format");
    
    time_t t = ::time(0);
    char date[20];
    strftime(date,sizeof(date),"%Y%m%d",localtime(&t));

    logger_ = spdlog::basic_logger_mt("CTPTD",log_path + "/ctptrd_" + date + ".log");
    logger_->set_level(get_log_level(level));
    logger_->set_pattern(format);
    logger_->flush_on(spdlog::level::info);
}

CtpTd::~CtpTd()
{
    api_->RegisterSpi(nullptr);
    if(api_)
    {
        api_->Release();
    }
}

void CtpTd::OnFrontConnected() 
{  
    connect_status_ = true;
    logger_->info(__FUNCTION__);
    req_auth();
}

void CtpTd::OnFrontDisconnected(int nReason)
{
    connect_status_ = false;
    logger_->error("{} (nReason) {} (ErrorMsg) {}",__FUNCTION__,nReason,disconnected_reason(nReason));
}

void CtpTd::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
    if (pRspInfo != nullptr && pRspInfo->ErrorID !=0 )
    {
        logger_->error("{} (ErrorId) {} (ErrorMsg) {}",__FUNCTION__,pRspInfo->ErrorID,pRspInfo->ErrorMsg);
    }
    else
    {
        logger_->info("{} success to authenticate",__FUNCTION__);
        login();
    }
}

void CtpTd::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo != nullptr && pRspInfo->ErrorID !=0 )
    {
        logger_->error("{} (ErrorId) {} (ErrorMsg) {}",__FUNCTION__,pRspInfo->ErrorID,pRspInfo->ErrorMsg);
    }
    else
    {
        login_status_ = true;
        logger_->info("{} (BrokerID) {} (UserID) {} (TradingDay) {} (FrontID) {} (SessionID) {}",
        __FUNCTION__,
        pRspUserLogin->BrokerID,
        pRspUserLogin->UserID,
        pRspUserLogin->TradingDay,
        pRspUserLogin->FrontID,
        pRspUserLogin->SessionID);
        
        session_id_ = pRspUserLogin->SessionID;
        front_id_ = pRspUserLogin->FrontID;
        order_ref_ = atoi(pRspUserLogin->MaxOrderRef);

        req_settlement_confirm();
    }
}

void CtpTd::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
    if (pRspInfo != nullptr && pRspInfo->ErrorID !=0)
    {
        logger_->error("{} (ErrorId) {} (ErrorMsg) {} (OrderRef) {} (Symbol) {}",__FUNCTION__,pRspInfo->ErrorID,pRspInfo->ErrorMsg,pInputOrder->OrderRef,pInputOrder->InstrumentID);
        OrderData order {};
        from_ctp(pInputOrder,order);
        gateway_->on_order(order);
    }
}

void CtpTd::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo != nullptr && pRspInfo->ErrorID !=0)
    {
        logger_->error("{} (ErrorId) {} (ErrorMsg) {} (OrderRef) {} (Symbol) {}",__FUNCTION__,pRspInfo->ErrorID,pRspInfo->ErrorMsg,pInputOrderAction->OrderRef,pInputOrderAction->InstrumentID);
    }
}

void CtpTd::OnRtnOrder(CThostFtdcOrderField *pOrder) 
{
    logger_->info("{} (OrderRef) {} (Symbol) {} {}@{} {} {} (Status) {} (Traded) {}",
                __FUNCTION__,
                pOrder->OrderRef,
                pOrder->InstrumentID,
                pOrder->LimitPrice,
                pOrder->VolumeTotalOriginal,
                ctp_direction(pOrder->Direction),
                ctp_comb_offset(pOrder->CombOffsetFlag),
                ctp_status(pOrder->OrderStatus,pOrder->OrderSubmitStatus),
                pOrder->VolumeTraded);
    
    OrderData order {};
    from_ctp(pOrder,order);
    gateway_->on_order(order);
}

void CtpTd::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
    logger_->info("{} (OrderRef) {} (Symbol) {} {}@{}",
                __FUNCTION__,
                pTrade->OrderRef,
                pTrade->InstrumentID,
                pTrade->Price,
                pTrade->Volume);
    
    TradeData trade {};
    from_ctp(pTrade,trade);
    gateway_->on_trade(trade);
}

void CtpTd::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo,int nRequestID, bool bIsLast) 
{
    if (pRspInfo != nullptr && pRspInfo->ErrorID !=0 )
    {
        logger_->error("{} (ErrorId) {} (ErrorMsg) {}",__FUNCTION__,pRspInfo->ErrorID,pRspInfo->ErrorMsg);
    }
    else
    {
        logger_->info("{} success to confirm settlement info",__FUNCTION__);
    }
}

void CtpTd::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
 if (pRspInfo != nullptr && pRspInfo->ErrorID != 0)
    {
        logger_->error("{} (ErrorId) {} (ErrorMsg) {}",__FUNCTION__,pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
}

void CtpTd::init()
{
    auto config = gateway_->trd_config();
    auto td_front = config.get_string("front");
    auto folder = config.get_string("folder");
    
    api_ = CThostFtdcTraderApi::CreateFtdcTraderApi((folder+"/").c_str());
    logger_->info("create ctp({}) td api with path: {}",api_->GetApiVersion(),folder);
    api_->RegisterSpi(this);
    api_->RegisterFront((char*)td_front.c_str());
    api_->SubscribePublicTopic(THOST_TERT_QUICK);
    api_->SubscribePrivateTopic(THOST_TERT_QUICK);
    api_->Init();
}

bool CtpTd::login()
{
    CThostFtdcReqUserLoginField login_field {};

    auto config = gateway_->trd_config();
    auto passwd = config.get_string("passwd");
    
    strcpy(login_field.UserID, investor_id_.c_str());
    strcpy(login_field.BrokerID, broker_id_.c_str());
    strcpy(login_field.Password, passwd.c_str());

    logger_->info("(UserID) {} (BrokerID) {}",investor_id_,broker_id_);
    auto ret = api_->ReqUserLogin(&login_field,++reqid_);

    if(ret)
    {
        logger_->error("{} failed to request login, (ErrorId) {}",__FUNCTION__,ret);
        return false;
    }
    else
    {
        return true;
    }
}

bool CtpTd::req_settlement_confirm()
{
    CThostFtdcSettlementInfoConfirmField req {};

    strcpy(req.InvestorID,investor_id_.c_str());
    strcpy(req.BrokerID,broker_id_.c_str());
    return (api_->ReqSettlementInfoConfirm(&req,++reqid_) == 0);
}

bool CtpTd::req_auth()
{
    CThostFtdcReqAuthenticateField req {};

    auto config = gateway_->trd_config();
    investor_id_ = config.get_string("username");
    broker_id_ = config.get_string("broker_id");

    auto app_id = config.get_string("app_id");
    auto auth_code = config.get_string("auth_code");
    auto product_info = config.get_string("product_info");
    
    strcpy(req.UserID,investor_id_.c_str());
    strcpy(req.BrokerID,broker_id_.c_str());
    strcpy(req.AppID,app_id.c_str());
    strcpy(req.AuthCode,auth_code.c_str());
    
    if(product_info.length() > 0)
    {
        strcpy(req.UserProductInfo,product_info.c_str());
    }

    auto ret = api_->ReqAuthenticate(&req,++reqid_);    
    if (ret != 0)
    {
        logger_->error("{} failed to req auth, (ErrorId) {}",__FUNCTION__,ret);
    }

    return (ret == 0);
}

int CtpTd::send_order(const OrderRequest& req)
{
    CThostFtdcInputOrderField order_field {};
    to_ctp(&order_field,req);
    strcpy(order_field.BrokerID,broker_id_.c_str());
    strcpy(order_field.InvestorID,investor_id_.c_str());

    auto order_id = std::to_string(session_id_ + order_ref_++);
    
    strcpy(order_field.OrderRef,order_id.c_str());
    int error_id = api_->ReqOrderInsert(&order_field,++reqid_);

    if (error_id != 0)
    {
        logger_->error("{} failed to insert order {}, (ErrorId) {}",__FUNCTION__,order_id,error_id);
        return -1;
    }
    else
    {
        logger_->info("{} (OrderRef) {} (Symbol) {}",__FUNCTION__,order_id,req.symbol);
        return atoi(order_id.c_str());
    }
}

bool CtpTd::cancel_order(const CancelRequest& req)
{
    CThostFtdcInputOrderActionField order_action {};

    strcpy(order_action.BrokerID, broker_id_.c_str());
    strcpy(order_action.InvestorID, investor_id_.c_str());
    strcpy(order_action.OrderRef,std::to_string(req.order_ref).c_str());
    strcpy(order_action.InstrumentID,req.symbol);
    strcpy(order_action.ExchangeID,req.exchange);

    order_action.FrontID = front_id_;
    order_action.SessionID = session_id_;
    order_action.ActionFlag = THOST_FTDC_AF_Delete;

    int error_id = api_->ReqOrderAction(&order_action,++reqid_);
    
    if (error_id != 0)
    {
        logger_->error("{} failed to cancel order {}, (ErrorId) {}",__FUNCTION__,req.order_ref,error_id);
        return false;
    }
    else
    {
        logger_->info("{} (OrderRef) {} (Symbol) {}",__FUNCTION__,req.order_ref,req.symbol);
        return true;
    }
}

SMARTQUANT_NAMESPACE_END
