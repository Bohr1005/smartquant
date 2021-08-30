#include "gateway/ctp/ctp_gateway.h"
#include "gateway/ctp/ctp_utils.h"
#include "object.h"
#include <ctime>

SMARTQUANT_NAMESPACE_START

CtpMd::CtpMd(CtpGateway* gateway) : gateway_(gateway),api_(nullptr)
{
    auto config = gateway_->mkt_config();
    auto log_path = config.get_string("log_path");
    auto level = config.get_string("log_level");
    auto format = config.get_string("format");
    
    time_t t = ::time(0);
    char date[20];
    strftime(date,sizeof(date),"%Y%m%d",localtime(&t));

    logger_ = spdlog::basic_logger_mt("CTPMD",log_path + "/ctpmkt_" + date + ".log");
    logger_->set_level(get_log_level(level));
    logger_->set_pattern(format);
    logger_->flush_on(spdlog::level::info);
};

CtpMd::~CtpMd()
{
    api_->RegisterSpi(nullptr);
    if(api_)
    {
        api_->Release();
    }
};

void CtpMd::OnFrontConnected()
{
    connect_status_ = true;
    logger_->info("{}",__FUNCTION__);
    login();
}

void CtpMd::OnFrontDisconnected(int nReason)
{
    connect_status_ = false;
    logger_->error("{} (nReason) {} (ErrorMsg) {}",__FUNCTION__,nReason,disconnected_reason(nReason));
}

void CtpMd::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo != nullptr && pRspInfo->ErrorID != 0)
    {
        logger_->error("{} (ErrorId) {} (ErrorMsg) {}", __FUNCTION__,pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {
        login_status_ = true;
        logger_->info("{} (BrokerID) {} (UserID) {} (TradingDay) {} ",__FUNCTION__, pRspUserLogin->BrokerID, pRspUserLogin->UserID, pRspUserLogin->TradingDay);

        if (!instruments_.empty())
        {
            for (auto symbol : instruments_)
            {
                subscribe(symbol);
            }
        }
    }
}

void CtpMd::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID,bool bIsLast)
{   

    if (pRspInfo != nullptr && pRspInfo->ErrorID != 0)
    {
        logger_->error("{} (error_id) {} (error_msg) {}",__FUNCTION__, pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    } 
    else
    {
        logger_->info("{} (Instrument) {}",__FUNCTION__, pSpecificInstrument->InstrumentID == nullptr ? "" : pSpecificInstrument->InstrumentID);
    }
}

void CtpMd::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
    if (pDepthMarketData != nullptr)
    {
        TickData tick {};
        from_ctp(pDepthMarketData,tick);
        gateway_->on_tick(tick);
    }
}

void CtpMd::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo != nullptr && pRspInfo->ErrorID != 0)
    {
        logger_->error("{} (ErrorId) {} (ErrorMsg) {}",__FUNCTION__, pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
}

void CtpMd::init()
{   
    auto config = gateway_->mkt_config();
    auto md_front = config.get_string("front");
    auto folder = config.get_string("folder");

    api_ = CThostFtdcMdApi::CreateFtdcMdApi((folder+"/").c_str());
    logger_->info("create ctp({}) md api with path: {}",api_->GetApiVersion(),folder);
    api_->RegisterSpi(this);
    api_->RegisterFront((char*)md_front.c_str());
    api_->Init();
}

bool CtpMd::login()
{   
    auto config = gateway_->mkt_config();
    auto username = config.get_string("username");
    auto passwd = config.get_string("passwd");
    auto broker_id = config.get_string("broker_id");
    
    CThostFtdcReqUserLoginField login_field {};

    strcpy(login_field.UserID, username.c_str());
    strcpy(login_field.BrokerID, broker_id.c_str());
    strcpy(login_field.Password, passwd.c_str());

    return (api_->ReqUserLogin(&login_field,++reqid_) == 0);
}

bool CtpMd::subscribe(const std::string& instrumentID)
{
    instruments_.insert(instrumentID);
	auto instrumentID_ = (char*) instrumentID.c_str();
	char* req[1] = { instrumentID_ };
	return (api_->SubscribeMarketData(req, 1) == 0);
}

SMARTQUANT_NAMESPACE_END
