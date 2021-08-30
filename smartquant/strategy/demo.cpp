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

class demo : public Template
{
public:
    demo(const Config& config) : Template(config)
    {}

    void init() override
    {}
    void on_event(const Event& event) override
    {
        
    }

private:
    
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
        auto strategy_ = new demo(child);
        engine.add_strategy(strategy_);
    }  
    engine.start();  // 启动
};