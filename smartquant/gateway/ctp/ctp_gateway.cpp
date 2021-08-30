#include "gateway/ctp/ctp_gateway.h"

SMARTQUANT_NAMESPACE_START

CtpGateway::CtpGateway(EventEngine* event_engine,Config& config) : Gateway(event_engine,config),type_(GatewayType::CTP),md_(this),td_(this)
{}

void CtpGateway::connect()
{
    md_.init();
    td_.init();
}

int CtpGateway::send_order(const OrderRequest& req) 
{
    return td_.send_order(req);
}

bool CtpGateway::cancel_order(const CancelRequest& req)
{
    return td_.cancel_order(req);
}

bool CtpGateway::subscribe(const std::string& instrumentID)
{
    return md_.subscribe(instrumentID);
}

bool CtpGateway::md_connect() const
{
    return md_.connect_status_ && md_.login_status_;
}

bool CtpGateway::td_connect() const
{
    return td_.connect_status_ && td_.login_status_;
}

GatewayType CtpGateway::gateway_type()
{
    return type_;
}

SMARTQUANT_NAMESPACE_END
