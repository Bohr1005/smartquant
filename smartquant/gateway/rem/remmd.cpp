#include "gateway/rem/rem_gateway.h"
#include "object.h"
#include "gateway/rem/rem_utils.h"
#include <ctime>

SMARTQUANT_NAMESPACE_START

RemMd::RemMd(RemGateway* gateway) : gateway_(gateway),api_(nullptr)
{
    auto config = gateway_->mkt_config();
    auto log_path = config.get_string("log_path");
    auto level = config.get_string("log_level");
    auto format = config.get_string("format");
    
    time_t t = ::time(0);
    char date[20];
    strftime(date,sizeof(date),"%Y%m%d",localtime(&t));

    logger_ = spdlog::basic_logger_mt("REMMD",log_path + "/remmkt_" + date + ".log");
    logger_->set_level(get_log_level(level));
    logger_->set_pattern(format);
    logger_->flush_on(spdlog::level::info);
};

RemMd::~RemMd()
{
    api_->DisConnServer();
    if(api_)
    {
        DestroyEESQuoteApi(api_);
    }
};

void RemMd::OnEqsConnected()
{
    connect_status_ = true;
    logger_->info("{}",__FUNCTION__);
    login();
}

void RemMd::OnEqsDisconnected()
{
    connect_status_ = false;
    logger_->error("{}",__FUNCTION__);
}

void RemMd::OnLoginResponse(bool bSuccess, const char* pReason)
{
    if (bSuccess)
    {
        login_status_ = true;
        logger_->info("{}",__FUNCTION__);
    }
   else
    {
        logger_->error("{} (Reason) {}", __FUNCTION__,pReason);
    }

}

void RemMd::OnSymbolRegisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess)
{
    if (bSuccess)
    {
        logger_->info("{} (Subscribe) {} (Type) {} success",__FUNCTION__,pSymbol,to_string(chInstrumentType));
    }
    else
    {
        logger_->error("{} (Subscribe) {} fail",__FUNCTION__,pSymbol);
    }
}

void RemMd::OnQuoteUpdated(EesEqsIntrumentType chInstrumentType, EESMarketDepthQuoteData* pDepthQuoteData)
{
    if (pDepthQuoteData != nullptr)
    {
        TickData tick {};
        from_rem(pDepthQuoteData,tick);
        gateway_->on_tick(tick);
    }
}

void RemMd::init()
{ 
    auto config = gateway_->mkt_config();
    auto md_ip = config.get_string("front_ip");
    auto md_port = config.get_string("front_port");

    api_ = CreateEESQuoteApi();
    logger_->info("create rem({}) md api");
    std::vector<EqsTcpInfo> v;
    EqsTcpInfo info;
    memset(info.m_eqsId,0,16);
    strcpy(info.m_eqsIp,md_ip.c_str());
    info.m_eqsPort = atoi(md_port.c_str());
    v.push_back(info);
    api_->ConnServer(v,this);
}

bool RemMd::login()
{  
    auto config = gateway_->mkt_config();
    auto username = config.get_string("username");
    auto passwd = config.get_string("passwd");

    EqsLoginParam login_info;
    strcpy(login_info.m_loginId,username.c_str());
    strcpy(login_info.m_password,passwd.c_str());
    api_->LoginToEqs(login_info);
}

bool RemMd::subscribe(const std::string& instrumentID)
{
    return true;
}

bool RemMd::subscribe(const std::string& instrumentID,InstrumentType type)
{
    EesEqsIntrumentType rem_type;
    to_rem(type,rem_type);
    api_->RegisterSymbol(rem_type,instrumentID.c_str());
    return true;
}


SMARTQUANT_NAMESPACE_END
