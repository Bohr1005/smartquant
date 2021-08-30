#ifndef SMARTQUANT_ALGO_H
#define SMARTQUANT_ALGO_H
#include "event/include/event.h"
#include "object.h"
#include "gateway/gateway.h"
#include <unordered_map>
#include <vector>

namespace SmartQuant
{
    class Instrument;

    class Template;

    class SmartOrder;

    class AlgoEngine
    {
    public:
        AlgoEngine(EventEngine* event_engine);

        std::unordered_map<std::string,TickData>& symbol_to_tick();

        void add_strategy(Template* strategy);

        void add_gateway(Gateway* gateway);

        Instrument* subscribe(Template* strategy,const std::string& instrumentID,GatewayType type);

        Instrument* subscribe(Template* strategy,const std::string& instrumentID,InstrumentType instrument_type,GatewayType type);

        int send_order(const OrderRequest& req);

        bool cancel_order(const CancelRequest& req);

        void register_handler(const EventType& event_type,EventEngine::callable callback);

        void register_default_handler();

        void add_timer(int latency,const std::any& data,EventEngine::callable callback);

        void process_on_tick(const Event& event);

        void process_on_order(const Event& event);

        void process_on_trade(const Event& event);

        void process_on_timer(const Event& event);

        void start();

        SmartOrder* id_to_smartorder(int64_t order_id);

        ~AlgoEngine();

    private:
        friend class SmartOrder;

        EventEngine* event_engine_;

        std::unordered_map<GatewayType,Gateway*> gateways_;

        std::unordered_map<std::string,TickData> symbol_to_tickdata_;

        std::vector<Template*> strategies_;

        std::unordered_map<std::string,std::vector<Template*>> symbol_to_strategy_impl_;

        std::vector<std::shared_ptr<Instrument>> instruments_;

        std::unordered_map<int64_t,SmartOrder*> id_to_smartorder_;
    };
} // namespace SmartQuant

#endif