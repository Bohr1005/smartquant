#ifndef SMARTQUANT_CTP_GATEWAY_H
#define SMARTQUANT_CTP_GATEWAY_H
#include "gateway/ctp/ctpmd.h"
#include "gateway/ctp/ctptd.h"
#include "declare.h"

namespace SmartQuant
{
    class CtpGateway : public Gateway
    {
    public:
        CtpGateway(EventEngine* event_engine,Config& config);

        void connect() override;

        int send_order(const OrderRequest& req) override;

        bool cancel_order(const CancelRequest& req) override;

        bool subscribe(const std::string& instrumentID) override;

        bool md_connect() const;

        bool td_connect() const;

        GatewayType gateway_type() override;
    
    private:
        GatewayType type_;

        CtpMd md_;

        CtpTd td_;
    };
} // namespace SmartQuant

#endif