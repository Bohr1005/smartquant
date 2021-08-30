#ifndef SMARTQUANT_CTPTD_H
#define SMARTQUANT_CTPTD_H
#include "api/ctp/ThostFtdcTraderApi.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "gateway/gateway.h"
#include "declare.h"
#include <string>

namespace SmartQuant
{
    class CtpGateway;

    class CtpTd : public CThostFtdcTraderSpi , public trader
    {
    public:
        CtpTd(CtpGateway* gateway);

        virtual ~CtpTd();
        
        void OnFrontConnected() override;

        void OnFrontDisconnected(int nReason) override;

        void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

        void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
 
        void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

        void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

        void OnRtnOrder(CThostFtdcOrderField *pOrder) override;

        void OnRtnTrade(CThostFtdcTradeField *pTrade) override;

        void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo,int nRequestID, bool bIsLast) override;

        void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

        void init() override;

        bool login() override;

        bool req_settlement_confirm() override;

        bool req_auth() override;

        int send_order(const OrderRequest& req) override;
        
        bool cancel_order(const CancelRequest& req) override;

    private:
        friend class CtpGateway;
        
        CtpGateway* gateway_;

        CThostFtdcTraderApi* api_;
    };
} // namespace SmartQuant


#endif