#include "algo/include/template.h"
#include "algo/include/smartorder.h"
#include "algo/include/instrument.h"
#include "algo/include/algo.h"
#include "gateway/ctp/ctp_gateway.h"
#include "gateway/stock_level2/l2_gateway.h"
#include <vector>
#include <utility>
#include <cmath>
#include <numeric>
#include <functional>
#include <unistd.h>
#include <chrono>

SMARTQUANT_NAMESPACE_START

class Recoder : public Template
{
public:
    Recoder(const Config& config) : Template(config)
    {}


    void init() override
    {
        auto future_symbol = config_.get_string("future");
        future_ = subscribe(future_symbol,GatewayType::CTP);

        auto spot_symbol = config_.get_string("spot");
        spot_ = subscribe(spot_symbol,GatewayType::L2);
    }

    void on_event(const Event& event) override
    {
        if (event.type() == EventType::EVENT_TICK)
        {
            auto data = event.data<TickData>();
            if (strcmp(data.symbol,future_->symbol()) == 0)
            {
                std::cout<<future_->datetime()<<','
                         <<future_->symbol()<<','
                         <<future_->last_price()<<','
                         <<spot_->symbol()<<','
                         <<spot_->last_price()<<std::endl;
            }
        }
    }

private:
    Instrument* future_;
    Instrument* spot_;
};

SMARTQUANT_NAMESPACE_END

int main(int argc,char** argv)
{
    using namespace SmartQuant;

    EventEngine event_engine(1024 * 1024);        // 创建事件驱动引擎，各种交易接口往事件驱动引擎里push行情，订单等数据(1024*1024是队列的大小)。
    AlgoEngine engine(&event_engine);      // 创建算法引擎。算法引擎会从事件驱动引擎里取出数据，推送到相应的策略里。
    
    auto path = argv[1];

    Config config(path);                   

    auto gateways = config.get_config("gateways");               
    auto ctp_config = gateways.get_config("ctp");        // 取ctp接口的配置
    auto l2_config = gateways.get_config("l2");          // 取股票全推行情接口的配置(全推行情本地合成指数)
    CtpGateway *ctp = new CtpGateway(&event_engine,ctp_config); // 创建ctp接口
    L2Gateway *l2 = new L2Gateway(&event_engine,l2_config);     // 创建全推行情接口
    engine.add_gateway(ctp);    // 把ctp接口加入到算法引擎
    engine.add_gateway(l2);     // 把全推行情加入到算法引擎
    engine.register_default_handler(); // 注册默认的回调函数(三个回调函数，行情，订单回报和成交回报)

    auto strategy_config = config.get_config("strategy_config");  // 读取策略的配置文件

    
    auto strategy_ = new Recoder(strategy_config);
    engine.add_strategy(strategy_);

    engine.start();  // 启动
};