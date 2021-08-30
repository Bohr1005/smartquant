#ifndef SMARTQUANT_REMTD_H
#define SMARTQUANT_REMTD_H
#include "api/rem/EesTraderApi.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "gateway/gateway.h"
#include "declare.h"
#include <string>

namespace SmartQuant
{
    class RemGateway;

    class RemTd : public EESTraderEvent , public trader
    {
    public:
        RemTd(RemGateway* gateway);

        virtual ~RemTd();
        
        virtual void OnConnection(ERR_NO errNo, const char* pErrStr ) override;

    	virtual void OnDisConnection(ERR_NO errNo, const char* pErrStr ) override;

        void init() override;

        bool login() override;

        bool req_settlement_confirm() override;

        bool req_auth() override;

        int send_order(const OrderRequest& req) override;
        
        bool cancel_order(const CancelRequest& req) override;

    private:
        friend class RemGateway;
        
        RemGateway* gateway_;

        EESTraderApi* api_;
    };
}

#endif
