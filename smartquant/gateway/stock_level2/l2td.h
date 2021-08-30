#ifndef SMARTQUANT_L2TD_H
#define SMARTQUANT_L2TD_H
#include "spdlog/sinks/basic_file_sink.h"
#include "gateway/gateway.h"
#include <string>
#include <zmq.hpp>
namespace SmartQuant
{
    class L2Gateway;

    class L2Td : public trader
    {
    public:
        L2Td(L2Gateway* gateway);

        virtual ~L2Td();

        void init() override;

        bool login() override;

        bool req_settlement_confirm() override;

        bool req_auth() override;

        int send_order(const OrderRequest& req) override;
        
        bool cancel_order(const CancelRequest& req) override;

    private:
        friend class L2Gateway;

        L2Gateway* gateway_;
    };
}

#endif