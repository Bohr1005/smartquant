#include "algo/include/template.h"
#include "algo/include/smartorder.h"
#include "algo/include/instrument.h"
#include "algo/include/algo.h"
#include "gateway/ctp/ctp_gateway.h"
#include <vector>
#include <utility>
#include <cmath>
#include <numeric>
#include <functional>
#include <csignal>
#include <unistd.h>

SMARTQUANT_NAMESPACE_START


class Pending : public Template
{
public:
    DEF_PARAM(int,vol,1);
    DEF_PARAM(int,kill_limit,450);
    
    Pending(const Config& config) : Template(config)
    {}

    void init() override
    {
        auto symbol = config_.get_string("symbol");
        instrument_ = subscribe(symbol,GatewayType::CTP);

        open_long_ = SmartOrder(instrument_,OrderType::LIMIT,OrderSide::LONG,Offset::OPEN);
        open_short_ = SmartOrder(instrument_,OrderType::LIMIT,OrderSide::SHORT,Offset::OPEN);
        close_long_ = SmartOrder(instrument_,OrderType::LIMIT,OrderSide::SHORT,Offset::CLOSETODAY);
        close_short_ = SmartOrder(instrument_,OrderType::LIMIT,OrderSide::LONG,Offset::CLOSETODAY);

        vol_ = config_.get_int("vol");
        close_dist_ = config_.get_int("close");
        auto kill_times = config_.get_int("kill_times");
        auto tick_size = config_.get_double("tick_size");
        instrument_->set_tick_size(tick_size);
        instrument_->set_kill_times(kill_times);
    }

    void on_event(const Event& event) override
    {
        
    }

private:
    Instrument* instrument_;
    SmartOrder open_long_;
    SmartOrder close_long_;
    SmartOrder open_short_;
    SmartOrder close_short_;
};

SMARTQUANT_NAMESPACE_END

int main(int argc,char** argv)
{
    using namespace SmartQuant;

    EventEngine event_engine(1024 * 1024);        // 创建事件驱动引擎，各种交易接口往事件驱动引擎里push行情，订单等数据(1024*1024是队列的大小)。
    AlgoEngine engine(&event_engine);      // 创建算法引擎。算法引擎会从事件驱动引擎里取出数据，推送到相应的策略里。
    
    auto path = argv[1];

    Config config(path);                   
    Config risk_config("/home/bohr/strategy/risk.json");

    auto gateways = config.get_config("gateways");               
    auto ctp_config = gateways.get_config("ctp");        // 取ctp接口的配置
    CtpGateway *ctp = new CtpGateway(&event_engine,ctp_config); // 创建ctp接口

    engine.add_gateway(ctp);    // 把ctp接口加入到算法引擎

    engine.register_default_handler(); // 注册默认的回调函数(三个回调函数，行情，订单回报和成交回报)

    auto strategy_config = config.get_config("strategy_config");  // 读取策略的配置文件

    auto children = strategy_config.children();
    for (auto child : children)
    {
        auto symbol = child.name();
        if (risk_config.contain(symbol))
        {
            auto kill_times = risk_config.get_int(symbol);
            child.set_value("kill_times",kill_times);
        }
        else
        {
            child.set_value("kill_times",0);
        }

        auto strategy_ = new Pending(child);
        engine.add_strategy(strategy_);
    }  
    
    stop_engine = [&engine](){ engine.~AlgoEngine(); };
    signal(SIGINT,handler);
    signal(SIGTERM,handler);

    engine.start();  // 启动
};