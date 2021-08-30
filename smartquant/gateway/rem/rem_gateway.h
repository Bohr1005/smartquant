#ifndef SMARTQUANT_REM_GATEWAY_H
#define SMARTQUANT_REM_GATEWAY_H
#include "gateway/rem/remmd.h"
#include "gateway/rem/remtd.h"
#include "declare.h"

namespace SmartQuant
{
    class RemGateway : public Gateway
    {
    public:
        RemGateway(EventEngine* event_engine,Config& config);

        void connect() override;

        int send_order(const OrderRequest& req) override;

        bool cancel_order(const CancelRequest& req) override;

        bool subscribe(const std::string& instrumentID) override;

        bool subscribe(const std::string& instrumentID,InstrumentType type) override;

        bool md_connect() const;

        bool td_connect() const;

        GatewayType gateway_type() override;
    
    private:
        GatewayType type_;

        RemMd md_;

        RemTd td_;
    };
} // namespace SmartQuant

#endif