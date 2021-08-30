#ifndef SMARTQUANT_CTPMD_H
#define SMARTQUANT_CTPMD_H
#include "api/ctp/ThostFtdcMdApi.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "gateway/gateway.h"
#include "declare.h"
#include <string>
#include <set>

namespace SmartQuant
{
    class CtpGateway;

    class CtpMd : public CThostFtdcMdSpi , public market_data
    {
    public:
        CtpMd(CtpGateway* gateway);

        virtual ~CtpMd();

        void OnFrontConnected() override;

        void OnFrontDisconnected(int nReason) override;

        void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

        void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID,bool bIsLast) override;

        void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) override;
       
        void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
        
        void init() override;
        
        bool login() override;

        bool subscribe(const std::string& instrumentID) override;

    private:
        friend class CtpGateway;

        CtpGateway* gateway_;

        CThostFtdcMdApi* api_;

        std::set<std::string> instruments_;
    };
} // namespace SmartQuant


#endif