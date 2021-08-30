#ifndef SMARTQUANT_L2MD_H
#define SMARTQUANT_L2MD_H
#include "spdlog/sinks/basic_file_sink.h"
#include "gateway/gateway.h"
#include "cppzmq/zmq.hpp"
#include "cppzmq/zmq_addon.hpp"
#include <string>
#include <thread>

namespace SmartQuant
{
    class L2Gateway;

    class L2Md: public market_data
    {
    public:
        L2Md(L2Gateway* gateway);

        virtual ~L2Md();

        void OnRtnDepthMarketData();

        void init() override;
        
        bool login() override;

        bool subscribe(const std::string& instrumentID) override;

    private:
        friend class L2Gateway;
        
        L2Gateway* gateway_;

        zmq::context_t ctx_;

        zmq::socket_t subscriber_;

        std::thread thread_;
    };
}

#endif