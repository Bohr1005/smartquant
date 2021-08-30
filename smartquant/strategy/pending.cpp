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

std::function<void()> stop_engine;

void handler(int signum)
{
    stop_engine();
    exit(-1);
}


class Pending : public Template
{
public:
    DEF_PARAM(int,vol,1);
    DEF_PARAM(int,far_dist,14);
    DEF_PARAM(int,pending_dist,9);
    DEF_PARAM(int,close_dist,4);
    DEF_PARAM(int,kill_limit,450);
    
    Pending(const Config& config) : Template(config)
    {}

    ~Pending()
    {
        killopen();
        usleep(5000);
    }

    void init() override
    {
        auto symbol = config_.get_string("symbol");
        instrument_ = subscribe(symbol,GatewayType::CTP);

        open_long_ = SmartOrder(instrument_,OrderType::LIMIT,OrderSide::LONG,Offset::OPEN);
        open_short_ = SmartOrder(instrument_,OrderType::LIMIT,OrderSide::SHORT,Offset::OPEN);
        close_long_ = SmartOrder(instrument_,OrderType::LIMIT,OrderSide::SHORT,Offset::CLOSETODAY);
        close_short_ = SmartOrder(instrument_,OrderType::LIMIT,OrderSide::LONG,Offset::CLOSETODAY);

        vol_ = config_.get_int("vol");
        far_dist_ = config_.get_int("far");
        pending_dist_ = config_.get_int("pending");
        close_dist_ = config_.get_int("close");
        auto kill_times = config_.get_int("kill_times");
        auto tick_size = config_.get_double("tick_size");
        instrument_->set_tick_size(tick_size);
        instrument_->set_kill_times(kill_times);
    }

    const Time& now()
    {
        return instrument_->datetime().time();
    }

    void open_long()
    {
        if (instrument_->net_pos() > 0)
            return;

        if (open_long_.is_active())
        {
            if (instrument_->bid_1_price() < instrument_->tick_up(open_long_.price(),close_dist_) ||
                instrument_->bid_1_price() > instrument_->tick_up(open_long_.price(),far_dist_))
                {
                    open_long_.kill();
                }
        }
        else
        {
            auto price = instrument_->tick_dn(instrument_->bid_1_price(),pending_dist_);
            open_long_.send(price,vol_);
        }
    }

    void open_short()
    {
        if (instrument_->net_pos() < 0)
            return;
        
        if (open_short_.is_active())
        {
            if (instrument_->ask_1_price() > instrument_->tick_dn(open_short_.price(),close_dist_) ||
                instrument_->ask_1_price() < instrument_->tick_dn(open_short_.price(),far_dist_))
                {
                    open_short_.kill();
                }
        }
        else
        {
            auto price = instrument_->tick_up(instrument_->ask_1_price(),pending_dist_);
            open_short_.send(price,vol_);
        }
    }

    void close_long()
    {
        if (instrument_->net_pos() <= 0)
            return;

        open_long_.kill();

        if (close_long_.is_active())
        {
            auto price = instrument_->tick_dn(instrument_->ask_1_price(),1);
            if (close_long_.price() > price)
            {
                close_long_.kill();
            }
        }
        else
        {
            auto price = instrument_->tick_dn(instrument_->ask_1_price(),1);
            close_long_.send(price,instrument_->net_pos());
        }
    }

    void close_short()
    {
        if (instrument_->net_pos() >= 0)
            return;
        
        open_short_.kill();

        if (close_short_.is_active())
        {
            auto price = instrument_->tick_up(instrument_->bid_1_price(),1);
            if (close_short_.price() < price)
            {
                close_short_.kill();
            }
        }
        else
        {
            auto price = instrument_->tick_up(instrument_->bid_1_price(),1);
            close_short_.send(price,-instrument_->net_pos());
        }
    }

    void force_close()
    {
        if (instrument_->net_pos() > 0)
        {
            if (close_long_.is_active() && close_long_.price() > instrument_->bid_1_price())
            {
                close_long_.kill();
            }
            else
            {
                auto price = instrument_->tick_dn(instrument_->bid_1_price(),2);
                close_long_.send(price,instrument_->net_pos());
            }
        }
        else if (instrument_->net_pos() < 0)
        {
            if (close_short_.is_active() && close_short_.price() < instrument_->ask_1_price())
            {
                close_short_.kill();
            }
            else
            {
                auto price = instrument_->tick_up(instrument_->ask_1_price(),2);
                close_short_.send(price,-instrument_->net_pos());
            }
        }
    };

    void killopen()
    {
        open_long_.kill();
        open_short_.kill();
    }

    bool is_price_limit()
    {
        return (instrument_->last_price() < instrument_->tick_up(instrument_->lower_limit_price(),30)) ||
               (instrument_->last_price() > instrument_->tick_dn(instrument_->upper_limit_price(),30));
    }

    void on_event(const Event& event) override
    {
        if (is_price_limit())
        {
            killopen();
            return;
        }
        

        if (instrument_->kill_times() >= kill_limit_)
        {
            if (open_long_.is_active() || open_short_.is_active())
            {
                killopen();
            }

            else if (instrument_->net_pos() == 0)
            {
                instrument_->set_tradable(false);
            }
            return;
        }

        if ((now() > Time(9,3,0) && now() < Time(10,14,0)) ||
            now() > Time(10,31,0) && now() < Time(11,29,0) ||
            now() > Time(13,1,0) && now() < Time(14,55,0) ||
            now() > Time(21,3,0) && now() < Time(22,58,0))
        {
            open_long();
            open_short();
            close_long();
            close_short();
        }

        else
        {
            killopen();
            force_close();
        }
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