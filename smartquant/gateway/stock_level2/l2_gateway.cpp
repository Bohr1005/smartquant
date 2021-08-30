#include "gateway/stock_level2/l2_gateway.h"

SMARTQUANT_NAMESPACE_START

L2Gateway::L2Gateway(EventEngine* event_engine,Config& config) : Gateway(event_engine,config),type_(GatewayType::L2),md_(this),td_(this)
{}

void L2Gateway::connect()
{
    md_.init();
    td_.init();
}

int L2Gateway::send_order(const OrderRequest& req)
{
    return td_.send_order(req);
}

bool L2Gateway::cancel_order(const CancelRequest& req) 
{
    return td_.cancel_order(req);
}

bool L2Gateway::subscribe(const std::string& instrumentID)
{
    return md_.subscribe(instrumentID);
}

bool L2Gateway::md_connect() const
{
    return md_.connect_status_;
}

bool L2Gateway::td_connect() const
{
    return td_.connect_status_;
}

GatewayType L2Gateway::gateway_type() 
{
    return type_;
}
SMARTQUANT_NAMESPACE_END