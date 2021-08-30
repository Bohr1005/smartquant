#include "algo/include/algo.h"
#include "declare.h"
#include "algo/include/template.h"
#include "algo/include/smartorder.h"
#include "algo/include/instrument.h"

SMARTQUANT_NAMESPACE_START

AlgoEngine::AlgoEngine(EventEngine* event_engine) : event_engine_(event_engine) {}

void AlgoEngine::add_strategy(Template* strategy)
{
    strategy->set_engine(this);
    strategies_.push_back(strategy);
}

std::unordered_map<std::string,TickData>& AlgoEngine::symbol_to_tick()
{
    return symbol_to_tickdata_;
}

void AlgoEngine::add_gateway(Gateway* gateway)
{
    if (gateways_.find(gateway->gateway_type()) == gateways_.end())
    {
        gateways_[gateway->gateway_type()] = gateway;
    }
}

Instrument* AlgoEngine::subscribe(Template* strategy,const std::string& instrumentID,GatewayType type)
{
    if(gateways_[type]->md_connect())
    {
        auto ret = gateways_[type]->subscribe(instrumentID);
        if (ret)
        {
            TickData tick {};
            symbol_to_tickdata_[instrumentID] = tick;
            strcpy(tick.symbol,instrumentID.c_str());
            tick.gateway = type;

            symbol_to_tickdata_[instrumentID] = tick;
            symbol_to_strategy_impl_[instrumentID].push_back(strategy);
            instruments_.push_back(std::make_shared<Instrument>(strategy,instrumentID));
            
            return instruments_.back().get();
        }
    }
    else
    {
        return nullptr;
    }
}

Instrument* AlgoEngine::subscribe(Template* strategy,const std::string& instrumentID,InstrumentType instrument_type,GatewayType type)
{
    if(gateways_[type]->md_connect())
    {
        auto ret = gateways_[type]->subscribe(instrumentID,instrument_type);
        if (ret)
        {
            TickData tick {};
            symbol_to_tickdata_[instrumentID] = tick;
            strcpy(tick.symbol,instrumentID.c_str());
            tick.gateway = type;

            symbol_to_tickdata_[instrumentID] = tick;
            symbol_to_strategy_impl_[instrumentID].push_back(strategy);
            instruments_.push_back(std::make_shared<Instrument>(strategy,instrumentID));
            
            return instruments_.back().get();
        }
    }
    else
    {
        return nullptr;
    }
}

int AlgoEngine::send_order(const OrderRequest& req)
{
    return gateways_[req.gateway]->send_order(req);
}

bool AlgoEngine::cancel_order(const CancelRequest& req)
{
    return gateways_[req.gateway]->cancel_order(req);
}

void AlgoEngine::register_handler(const EventType& event_type,EventEngine::callable callback)
{
    event_engine_->register_handler(event_type,callback);
}

void AlgoEngine::register_default_handler()
{
    event_engine_->register_handler(EventType::EVENT_TICK,std::function<void(const Event&)>(std::bind(&AlgoEngine::process_on_tick,this,std::placeholders::_1)));
    event_engine_->register_handler(EventType::EVENT_ORDER,std::function<void(const Event&)>(std::bind(&AlgoEngine::process_on_order,this,std::placeholders::_1)));
    event_engine_->register_handler(EventType::EVENT_TRADE,std::function<void(const Event&)>(std::bind(&AlgoEngine::process_on_trade,this,std::placeholders::_1)));
    event_engine_->register_handler(EventType::EVENT_TIMER,std::function<void(const Event&)>(std::bind(&AlgoEngine::process_on_timer,this,std::placeholders::_1)));
}

void AlgoEngine::add_timer(int latency,const std::any& data,EventEngine::callable callback)
{
    event_engine_->add_timer(latency,data,callback);
}

void AlgoEngine::process_on_tick(const Event& event)
{
    auto&& tickdata = event.data<TickData>();
    symbol_to_tickdata_[tickdata.symbol] = tickdata;
    auto strategies = symbol_to_strategy_impl_[tickdata.symbol];
    for (auto strategy : strategies)
    {
        strategy->on_event(event);
    }
}

void AlgoEngine::process_on_order(const Event& event)
{
    auto orderdata = event.data<OrderData>();
    auto order_id = orderdata.order_id;
    auto status = orderdata.status;

    if (id_to_smartorder_.find(order_id) != id_to_smartorder_.end())
    {  
        auto smartorder = id_to_smartorder_[order_id];
        if (smartorder->order_id_ != order_id)
            return;

        auto strategy = smartorder->instrument_->strategy_impl_;

        switch (status)
        {
        case Status::PARTIALFILLEDNOTACTIVE:
            smartorder->status_ = Status::PARTIALFILLEDNOTACTIVE;
            return;    

        case Status::PARTIALFILLEDACTIVE:
            smartorder->status_ = Status::PARTIALFILLEDACTIVE;
            return;

        case Status::FILLED:
            smartorder->status_ = Status::FILLED;
            return;
            
        case Status::ERROR:
            smartorder->status_ = Status::ERROR;
            smartorder->is_active_ = false;
            return;

        case Status::REJECT:
            smartorder->status_ = Status::REJECT;
            smartorder->is_active_ = false;
            return;

        case Status::SUBMITTED:
            smartorder->status_ = Status::SUBMITTED;
            strategy->on_event(event);
            break;  

        case Status::PENDING:
            smartorder->status_ = Status::PENDING;
            strategy->on_event(event);
            break;

        case Status::CANCELLED:
            ++(smartorder->instrument_->kill_times_);
            smartorder->is_active_ = false;
            strategy->on_event(event);
            break;    
        }
    }
}


void AlgoEngine::process_on_trade(const Event& event)
{
    auto tradedata = event.data<TradeData>();
    auto order_id = tradedata.order_id;
    
    if (id_to_smartorder_.find(order_id) != id_to_smartorder_.end())
    {
        auto smartorder = id_to_smartorder_[order_id];
        
        if (smartorder->order_id_ != order_id)
            return;

        auto offset = tradedata.offset;
        auto side = tradedata.side;
        auto volume = tradedata.volume;
        auto instrument = smartorder->instrument_;
        auto strategy = instrument->strategy_impl_;

        if (smartorder->status_ == Status::PARTIALFILLEDNOTACTIVE || 
            smartorder->status_ == Status::FILLED)
            {
                smartorder->is_active_ = false;
            }

        switch (side)
        {
        case OrderSide::LONG:
            if (offset == Offset::OPEN)
            {
                instrument->today_long_pos_ += volume;
            }
            else if (offset == Offset::CLOSETODAY)
            {
                instrument->today_short_pos_ -= volume;
            }
            else if (offset == Offset::CLOSEYESTERDAY)
            {
                instrument->yst_short_pos_ -= volume;
            }
            else if (offset == Offset::CLOSE)
            {
                if (instrument->yst_short_pos_ >= volume)
                {
                    instrument->yst_short_pos_ -= volume;
                }
                else
                {
                    auto diff = volume - instrument->yst_short_pos_;
                    instrument->today_short_pos_ -= diff;
                    instrument->yst_short_pos_ = 0;
                }
            }
            break;
        
        case OrderSide::SHORT:
            if (offset == Offset::OPEN)
            {
                instrument->today_short_pos_ += volume;
            }
            else if (offset == Offset::CLOSETODAY)
            {
                instrument->today_long_pos_ -= volume;
            }
            else if (offset == Offset::CLOSEYESTERDAY)
            {
                instrument->yst_long_pos_ -= volume;
            }
            else if (offset == Offset::CLOSE)
            {
                if (instrument->yst_long_pos_ >= volume)
                {
                    instrument->yst_long_pos_ -= volume;
                }
                else
                {
                    auto diff = volume - instrument->yst_long_pos_;
                    instrument->today_long_pos_ -= diff;
                    instrument->yst_long_pos_ = 0;
                }
            }
            break;
        }

        strategy->on_event(event);
    }   
}

void AlgoEngine::process_on_timer(const Event& event)
{
    event.invoke_callback();
}

void AlgoEngine::start()
{
    for (auto gateway : gateways_)
    {
        auto it = gateway.second;
        it->connect();
        while((!it->md_connect() && !it->td_connect()))
        {
            sleep(1);
        }
    }

    for (auto strategy : strategies_)
    {
        strategy->init();
    }

    event_engine_->start();
}

SmartOrder* AlgoEngine::id_to_smartorder(int64_t order_id)
{
    if (id_to_smartorder_.find(order_id) != id_to_smartorder_.end())
    {
        return id_to_smartorder_[order_id];
    }
    else
    {
        return nullptr;
    }
}

AlgoEngine::~AlgoEngine()
{
    event_engine_->stop();

    for (auto strategy : strategies_)
    {
        delete strategy;
    }

    for (auto gateway : gateways_)
    {
        delete gateway.second;
    }
}

SMARTQUANT_NAMESPACE_END