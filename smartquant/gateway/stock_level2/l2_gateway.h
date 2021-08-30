#ifndef SMARTQUANT_L2_GATEWAY_H
#define SMARTQUANT_L2_GATEWAY_H
#include "gateway/stock_level2/l2md.h"
#include "gateway/stock_level2/l2td.h"
#include "declare.h"

namespace SmartQuant
{
    class L2Gateway : public Gateway
    {
    public:
        L2Gateway(EventEngine* event_engine,Config& config);

        void connect() override;

        int send_order(const OrderRequest& req) override;

        bool cancel_order(const CancelRequest& req) override;

        bool subscribe(const std::string& instrumentID) override;

        bool md_connect() const;

        bool td_connect() const;

        GatewayType gateway_type() override;

    private:
        GatewayType type_;

        L2Md md_;

        L2Td td_; 
    };
} // namespace SmartQuant


#endif