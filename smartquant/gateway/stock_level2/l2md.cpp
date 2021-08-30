#include "gateway/stock_level2/l2_gateway.h"
#include "object.h"

SMARTQUANT_NAMESPACE_START

L2Md::L2Md(L2Gateway* gateway) : gateway_(gateway),ctx_(1),subscriber_(ctx_,ZMQ_SUB)
{}

L2Md::~L2Md()
{}

void L2Md::OnRtnDepthMarketData()
{
    while (true)
    {
        std::vector<zmq::message_t> recv_msgs;
        auto ret = zmq::recv_multipart(subscriber_,std::back_inserter(recv_msgs));
        TickData tick {};
        tick.datetime = DateTime::now();
        strcpy(tick.symbol,recv_msgs[0].to_string().c_str());
        tick.last_price = atof(recv_msgs[1].to_string().c_str());
        gateway_->on_tick(tick);
    }
}

void L2Md::init()
{
    auto config = gateway_->mkt_config();
    auto md_front_1 = config.get_string("front_1");
    auto md_front_2 = config.get_string("front_2");
    subscriber_.connect(md_front_1);
    subscriber_.connect(md_front_2);

    thread_ = std::thread(&L2Md::OnRtnDepthMarketData,this);
    connect_status_ = true;
}

bool L2Md::login() 
{
    return true;
}

bool L2Md::subscribe(const std::string& instrumentID)
{
    subscriber_.set(zmq::sockopt::subscribe,instrumentID);
    return true;
}

SMARTQUANT_NAMESPACE_END