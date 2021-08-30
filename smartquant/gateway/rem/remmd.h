#ifndef SMARTQUANT_REMMD_H
#define SMARTQUANT_REMMD_H
#include "api/rem/EESQuoteApi.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "gateway/gateway.h"
#include "declare.h"
#include <string>

namespace SmartQuant
{
    class RemGateway;

    class RemMd : public EESQuoteEvent , public market_data
    {
    public:
        RemMd(RemGateway* gateway);

        virtual ~RemMd();

        void OnEqsConnected() override;

        void OnEqsDisconnected() override;

        void OnLoginResponse(bool bSuccess, const char* pReason) override;
        
        void OnSymbolRegisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess) override;

        void OnQuoteUpdated(EesEqsIntrumentType chInstrumentType, EESMarketDepthQuoteData* pDepthQuoteData) override;

        void init() override;
        
        bool login() override;

        bool subscribe(const std::string& instrumentID) override;

        bool subscribe(const std::string& instrumentID,InstrumentType type);

    private:
        friend class RemGateway;

        RemGateway* gateway_;

        EESQuoteApi* api_;
    };
}

#endif