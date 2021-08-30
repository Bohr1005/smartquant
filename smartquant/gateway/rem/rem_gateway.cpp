#include "gateway/rem/rem_gateway.h"

SMARTQUANT_NAMESPACE_START

RemGateway::RemGateway(EventEngine* event_engine,Config& config) : Gateway(event_engine,config),type_(GatewayType::CTP),md_(this),td_(this)
{}

void RemGateway::connect()
{
    md_.init();
    td_.init();
}

int RemGateway::send_order(const OrderRequest& req) 
{
    return td_.send_order(req);
}

bool RemGateway::cancel_order(const CancelRequest& req)
{
    return td_.cancel_order(req);
}

bool RemGateway::subscribe(const std::string& instrumentID)
{
    return md_.subscribe(instrumentID);
}

bool RemGateway::md_connect() const
{
    return md_.connect_status_ && md_.login_status_;
}

bool RemGateway::td_connect() const
{
    return td_.connect_status_ && td_.login_status_;
}

GatewayType RemGateway::gateway_type()
{
    return type_;
}

SMARTQUANT_NAMESPACE_END