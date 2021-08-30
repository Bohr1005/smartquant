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
#include <csignal>

SMARTQUANT_NAMESPACE_START

// Indicator 专门用来计算策略需要的技术指标
class Indicator
{
public:
    Indicator(int window=15) : window_(window),spread_(window,0.0) {}

    double inited() const { return n_ >= window_; }

    double last_spread() const { return last_spread_; }

    double pre_spread() const { return pre_spread_; }

    double mean() const 
    {
        return mean_;
    }

    double pre_mean() const { return pre_mean_; }
  
    void update_spread(double v) 
    {   
        pre_spread_ = last_spread_;
        last_spread_ = v;
        sum_ += v;
        std::swap(v,spread_[n_++ % spread_.size()]);
        sum_ -= v;

        pre_mean_ = mean_;
        mean_ = sum_ / spread_.size();
    }

    int n() const
    {
        return n_;
    }
    
private:
    int window_;

    std::vector<double> spread_;
    int n_;

    double sum_ { 0.0 };
    double mean_ { 0.0 };
    double pre_mean_ { 0.0 };

    double last_spread_ { 0.0 };
    double pre_spread_ { 0.0 };
};

/////////////////////////////////////////////////////////////

class IndexArb_v2 : public Template
{
public:
    DEF_PARAM(double,long_thres,0.0);
    DEF_PARAM(double,short_thres,0.0);
    DEF_PARAM(int,window,15);
    DEF_PARAM(int,vol,1);
    DEF_PARAM(int,pending_dist,5);
    DEF_PARAM(bool,use_close_offset,false);
    DEF_PARAM(int,exit_dist,0);
    DEF_PARAM(double,long_exit_thres,0.0);
    DEF_PARAM(double,short_exit_thres,0.0);

    IndexArb_v2(const Config& config) : Template(config)
    {}

    ~IndexArb_v2() {}

    void init() override
    {   // 配置参数，订阅标的，设置下单对象。
        auto future_symbol = config_.get_string("future");
        auto spot_symbol = config_.get_string("spot");

        future_ = subscribe(future_symbol,GatewayType::CTP);
        spot_ = subscribe(spot_symbol,GatewayType::L2);

        long_thres_ = config_.get_double("long_thres");
        short_thres_ = config_.get_double("short_thres");
        window_ = config_.get_int("window");
        vol_ = config_.get_int("vol");
        pending_dist_ = config_.get_int("pending_dist");
        exit_dist_ = config_.get_int("exit_dist");
        long_exit_thres_ = config_.get_double("long_exit_thres");
        short_exit_thres_ = config_.get_double("short_exit_thres");
        use_close_offset_ = config_.get_bool("use_close_offset");

        auto tick_size = config_.get_double("tick_size");
        future_->set_tick_size(tick_size);

        open_long_ = SmartOrder(future_,OrderType::LIMIT,OrderSide::LONG,Offset::OPEN);
        open_short_ = SmartOrder(future_,OrderType::LIMIT,OrderSide::SHORT,Offset::OPEN);
        close_long_ = SmartOrder(future_,OrderType::LIMIT,OrderSide::SHORT,Offset::OPEN);
        close_short_ = SmartOrder(future_,OrderType::LIMIT,OrderSide::LONG,Offset::OPEN);

        if (use_close_offset_)
        {
            open_long_.set_offset(Offset::CLOSE);
            open_short_.set_offset(Offset::CLOSE);
            close_long_.set_offset(Offset::CLOSE);
            close_short_.set_offset(Offset::CLOSE);
        }
    }

    void on_tick(const TickData& tick) 
    {
        if (future_->datetime().time() < Time(9,31,0) || future_->datetime().time() > Time(14,55,0))
            return;

        if (strcmp(tick.symbol,future_->symbol()) == 0)
        {
            auto bid_spread = spot_->last_price() - future_->bid_1_price();
            auto ask_spread = spot_->last_price() - future_->ask_1_price();

            bid_.update_spread(bid_spread);
            ask_.update_spread(ask_spread);
        }
    }

    bool is_price_ok() const 
    {
        return future_->is_price_ok() && spot_->last_price() > 0;
    }
    
    void killall()
    {
        open_long_.kill();
        open_short_.kill();
        close_long_.kill();
        close_short_.kill();
    }

    void open_long()
    {
        if (future_->net_pos() > 0)
            return;

        auto spread = bid_.last_spread();
        auto pre_spread = bid_.pre_spread();
        auto ma = bid_.mean();
        auto pre_ma = bid_.pre_mean();

        auto long_signal = spread - ma >= long_thres_;
        auto pre_long_signal = pre_spread - pre_ma >= long_thres_;

        std::cout<<__FUNCTION__<<','<<spread<<','<<pre_spread<<','<<ma<<','<<pre_ma<<std::endl;

        if (open_long_.is_active())
        {
            if (!long_signal || !pre_long_signal)
            {
                open_long_.kill();
            }
        }
        else if (long_signal && pre_long_signal)
        {
            auto prc = std::min(future_->last_price(),future_->bid_1_price());
            auto our_price = future_->tick_dn(prc,pending_dist_);
            open_long_.send(our_price,vol_);
        }
    }

    void open_short()
    {
        if (future_->net_pos() < 0)
            return;
            
        auto spread = ask_.last_spread();
        auto pre_spread = ask_.pre_spread();
        auto ma = ask_.mean();
        auto pre_ma = ask_.pre_mean();

        auto short_signal = spread - ma <= short_thres_;
        auto pre_short_signal = pre_spread - pre_ma <= short_thres_;

        std::cout<<__FUNCTION__<<','<<spread<<','<<pre_spread<<','<<ma<<','<<pre_ma<<std::endl;

        if (open_short_.is_active())
        {
            if (!short_signal || !pre_short_signal)
            {
                open_short_.kill();
            }
        }
        else if (short_signal && pre_short_signal)
        {
            auto prc = std::max(future_->ask_1_price(),future_->last_price());
            auto our_price = future_->tick_up(prc,pending_dist_);
            open_short_.send(our_price,vol_);
        }
    }

    void close_long()
    {
        if (future_->net_pos() <= 0)
        {
            close_long_.kill();
            return;
        }
        
        open_long_.kill();

        auto thres = ask_.mean() + long_exit_thres_;
        auto pre_thres = ask_.pre_mean() + long_exit_thres_;
        auto spread = ask_.last_spread();
        auto pre_spread = ask_.pre_spread();

        auto close_signal = spread <= thres;
        auto pre_close_signal = pre_spread <= pre_thres;

        if (close_long_.is_active())
        {
            if(close_signal && pre_close_signal)
            {
                if (close_long_.price() > future_->tick_up(future_->ask_1_price(),exit_dist_))
                {
                    close_long_.kill();
                }
            }
            else
            {
                close_long_.kill();
            }
        }
        else if (close_signal && pre_close_signal)
        {
            auto prc = future_->tick_up(future_->ask_1_price(),exit_dist_);
            close_long_.send(prc,future_->net_pos());
        }
    }

    void close_short()
    {
        if (future_->net_pos() >= 0)
        {
            close_short_.kill();
            return;
        }

        open_short_.kill();
        auto thres = bid_.mean() + short_exit_thres_;
        auto pre_thres = bid_.pre_mean() + short_exit_thres_;
        auto spread = bid_.last_spread();
        auto pre_spread = bid_.pre_spread();

        auto close_signal = spread >= thres;
        auto pre_close_signal = pre_spread >= pre_thres;

        if (close_short_.is_active())
        {
            if (close_signal && pre_close_signal)
            {
                if (close_short_.price() < future_->tick_dn(future_->bid_1_price(),exit_dist_))
                {
                    close_short_.kill();
                }
                else
                {
                    close_short_.kill();
                }
            }
        }
        else
        {
            auto prc = future_->tick_dn(future_->bid_1_price(),exit_dist_);
            close_short_.send(-future_->net_pos(),prc);
        }
    }

    void force_close()
    {
        if (future_->net_pos() > 0)
        {
            if (close_long_.is_active() && close_long_.price() > future_->bid_1_price())
            {
                close_long_.kill();
            }
            else
            {
                close_long_.send(future_->bid_1_price(),future_->net_pos());
            }
        }

        else if (future_->net_pos() < 0)
        {
            if (close_short_.is_active() && close_short_.price() < future_->ask_1_price())
            {
                close_short_.kill();
            }
            else
            {
                close_short_.send(future_->ask_1_price(),-future_->net_pos());
            }
        }
    }

    void on_event(const Event& event) override
    {
        if (!is_price_ok())
            return;

        switch (event.type())
        {
        case EventType::EVENT_TICK:
            on_tick(event.data<TickData>());
            break;
        }

        if (future_->datetime().time() > Time(9,31,0) && future_->datetime().time() < Time(14,55,0))
        {
            if (!bid_.inited() || !ask_.inited())
                return;
            
            // std::cout<<DateTime::now()<<",future:"<<future_->last_price()<<",ask:"<<future_->ask_1_price()<<",bid:"<<future_->bid_1_price()<<",spot:"<<spot_->last_price()<<','<<"spread:"<<indicator_.last_spread()<<",spread_ma:"<<indicator_.mean()<<','<<indicator_.last_spread()-indicator_.mean()<<std::endl;

            open_long();
            open_short();
            close_long();
            close_short();
        }

        else if (DateTime::now().time() > Time(14,55,0))
        {
            force_close();
        }

        else
        {
            killall();
        }
    }

private:
    Instrument *future_;
    Instrument *spot_;

    SmartOrder open_long_;
    SmartOrder close_long_;
    SmartOrder open_short_;
    SmartOrder close_short_;

    Indicator bid_;
    Indicator ask_;

    double trade_price_;

    bool close_flag_ { false };

    int last_send_ { 0 };

};

SMARTQUANT_NAMESPACE_END

int main()
{
    using namespace SmartQuant;

    EventEngine event_engine(1024 * 1024);        // 创建事件驱动引擎，各种交易接口往事件驱动引擎里push行情，订单等数据(1024*1024是队列的大小)。
    AlgoEngine engine(&event_engine);      // 创建算法引擎。算法引擎会从事件驱动引擎里取出数据，推送到相应的策略里。
    
    Config config("./config.json");               

    auto gateways = config.get_config("gateways");               
    auto ctp_config = gateways.get_config("ctp");        // 取ctp接口的配置
    auto l2_config = gateways.get_config("l2");          // 取股票全推行情接口的配置(全推行情本地合成指数)
    CtpGateway *ctp = new CtpGateway(&event_engine,ctp_config); // 创建ctp接口
    L2Gateway *l2 = new L2Gateway(&event_engine,l2_config);     // 创建全推行情接口
    engine.add_gateway(ctp);    // 把ctp接口加入到算法引擎
    engine.add_gateway(l2);     // 把全推行情加入到算法引擎

    engine.register_default_handler(); // 注册默认的回调函数(三个回调函数，行情，订单回报和成交回报)
    auto strategy_config = config.get_config("strategy_config");  // 读取策略的配置文件
    auto children = strategy_config.children();
    for (auto child : children)
    {
        auto strategy_ = new IndexArb_v2(child);
        engine.add_strategy(strategy_);
    }  
    engine.start();  // 启动

}

