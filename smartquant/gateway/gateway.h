#ifndef SMARTQUANT_GATEWAY_H
#define SMARTQUANT_GATEWAY_H
#include "event/include/event.h"
#include "object.h"
#include "utils.h"

namespace SmartQuant
{
    class market_data
    {
    public:
        market_data() : reqid_(0), connect_status_(false),login_status_(false) {}

        virtual void init() = 0;
        
        virtual bool login() = 0;

        virtual bool subscribe(const std::string& instrumentID) = 0;

        virtual bool subscribe(const std::string& instrumentID,InstrumentType type) { return true; };

    protected:
        int reqid_;

        bool connect_status_;

        bool login_status_;

        std::shared_ptr<spdlog::logger> logger_;
    };

/////////////////////////////////////////////////////////////

    class trader
    {
    public:
        trader() : reqid_(0), connect_status_(false),login_status_(false) {}

        virtual void init() = 0;

        virtual bool login() = 0;

        virtual bool req_settlement_confirm() = 0;

        virtual bool req_auth() = 0;

        virtual int send_order(const OrderRequest& req) = 0;
        
        virtual bool cancel_order(const CancelRequest& req) = 0;

    protected:
        int reqid_;

        int order_ref_;
        
        int front_id_;
        
        int session_id_;

        bool connect_status_;

        bool login_status_;

        std::string investor_id_;

        std::string broker_id_;

        std::shared_ptr<spdlog::logger> logger_;
    };

/////////////////////////////////////////////////////////////

    class Gateway
    {
    public:
        Gateway(EventEngine* event_engine,Config& config) : event_engine_(event_engine),config_(config)
        {};

        void on_tick(TickData& data)
        {
            Event e(EventType::EVENT_TICK,data);
            event_engine_->push(e);
        }

        void on_trade(TradeData& data)
        {
            Event e(EventType::EVENT_TRADE,data);
            event_engine_->push(e);
        }

        void on_order(OrderData& data)
        {
            Event e(EventType::EVENT_ORDER,data);
            event_engine_->push(e);
        }
        
        virtual GatewayType gateway_type() = 0;

        virtual void connect() = 0;

        virtual bool subscribe(const std::string& instrumentID) = 0;

        virtual bool subscribe(const std::string& instrumentID,InstrumentType type) { return true; };

        virtual int send_order(const OrderRequest& req) = 0;

        virtual bool cancel_order(const CancelRequest& req) = 0;

        virtual bool td_connect() const = 0;

        virtual bool md_connect() const = 0;

        Config mkt_config()
        { return config_.get_config("mkt"); };

        Config trd_config()
        { return config_.get_config("trd"); };

    protected:
        EventEngine* event_engine_;

        Config config_;
    };
} // namespace SmartQuant


#endif