#include "algo/include/algo.h"
#include "declare.h"
#include "algo/include/template.h"

SMARTQUANT_NAMESPACE_START

Template::Template(const Config& config) : engine_(nullptr),config_(config)
{}

Template::~Template() {}

void Template::set_engine(AlgoEngine* engine)
{
    engine_ = engine;
}

Instrument* Template::subscribe(const std::string& symbol, const GatewayType& type)
{
    return engine_->subscribe(this,symbol,type);
}

SmartOrder* Template::id_to_smartorder(int64_t order_id)
{
    return engine_->id_to_smartorder(order_id);
}

void Template::add_timer(int latency,const std::any& data,EventEngine::callable callback)
{
    engine_->add_timer(latency,data,callback);
}

SMARTQUANT_NAMESPACE_END