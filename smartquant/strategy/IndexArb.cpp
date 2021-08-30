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

std::function<void()> stop_engine;

void handler(int signum)
{
    stop_engine();
    exit(-1);
}

SMARTQUANT_NAMESPACE_START

// Indicator 专门用来计算策略需要的技术指标
class Indicator
{
public:
    Indicator(int window=1800) : window_(window),
                                 margin_cache_(0.0),
                                 margin_index_cache_(0.0),
                                 margin_ewma_(0.0),
                                 m_margin_index_return_(0.0),
                                 ma_margin_index_return_(0.0),
                                 sig_list_(window,0.0),
                                 n_(0),
                                 short_timer_(-window),
                                 long_timer_(-window)
    {
        margin_list_.reserve(32768);                // reserve预留一些内存空间，防止盘中的时候std::vector的capacity不足，重新分配内存影响效率。
        margin_index_list_.reserve(32768);
    }

    void span_exp_average() 
    {
        double alpha = 2.0 / (n_ + 1);
        auto numerator = 0.0;
        auto denominator = 0.0;
        for (int i=0;i<margin_list_.size();++i)
        {
            numerator += (margin_list_[i] * std::pow((1 - alpha),n_ - i));
            denominator += std::pow(1 - alpha,i);
        }
        margin_ewma_ = numerator / denominator;
    }

    void exp_average(double v,double factor=0.0005) 
    {
        m_margin_index_return_ = (1 - factor) * m_margin_index_return_ + factor * v;
        if (n_ > 0)
            ma_margin_index_return_ = m_margin_index_return_ / (1 - std::pow(1 - factor,n_));
        else
            ma_margin_index_return_ = m_margin_index_return_;
    }

    double margin_pre() const { return margin_cache_; }

    double margin_index_pre() const { return margin_index_cache_; }

    double ma_margin() const { return margin_ewma_; }

    double ma_margin_index_return() const { return ma_margin_index_return_; }

    double is_ready() const { return n_ >= 2; }

    double std() const 
    {
        if (n_ < window_)
        {
            auto mean = std::accumulate(sig_list_.begin(),sig_list_.end(),0.0) / n_;
            auto var = 0.0;
            for (int i=0;i<n_;++i)
            {
                var += std::pow((sig_list_[i] - mean),2);
            }
            return std::sqrt(var / n_);
        }
        else
        {
            auto mean = std::accumulate(sig_list_.begin(),sig_list_.end(),0.0) / sig_list_.size();
            auto var = std::accumulate(sig_list_.begin(),sig_list_.end(),0.0,[mean](double sum,double v){ return sum + (v - mean) * (v - mean);});
            return std::sqrt(var / (sig_list_.size()));
        }
    }

    int n() const { return n_; }

    int short_timer() const { return short_timer_; }

    int long_timer() const { return long_timer_; }

    void set_short_timer(int v) { short_timer_ = v; }

    void set_long_timer(int v) { long_timer_ = v; }
  
    void update_margin(double v) 
    {
        if (n_ > 0)
            margin_cache_ = margin_list_.back();
        margin_list_.emplace_back(v);
        span_exp_average();
    }

    void update_margin_index(double v) 
    {
        if (n_ > 0)
            margin_index_cache_ = margin_index_list_.back();
        margin_index_list_.emplace_back(v);
    }

    void update_margin_index_return(double v) 
    {
        exp_average(v,0.0015);
    }

    void update_sig_list(double v) 
    {
        auto value = v - margin_ewma_;
        std::swap(value,sig_list_[n_++ % sig_list_.size()]);
    }

private:
    int window_;
    double margin_cache_;
    double margin_index_cache_;

    double margin_ewma_;
    double m_margin_index_return_;
    double ma_margin_index_return_;

    std::vector<double> margin_list_;
    std::vector<double> margin_index_list_;
    std::vector<double> sig_list_;
    int n_;

    int short_timer_;
    int long_timer_;

};

/////////////////////////////////////////////////////////////

// 策略部分，继承策略模板Template，实现两个纯虚函数init和on_event,init用来做策略初始化，只运行一次。
// on_event是策略运行的核心，在有事件发生时，例如：行情，order回报会调用
class IndexArb : public Template
{
public:
    // DEF_PARAM是一个宏函数，用来定义策略需要用到的参数，需要填入三个参数:参数类型，参数名字，初始值。会被替换成2个函数和一个变量值。
    // 例如DEF_PARAM(int,time_gap,500)会被替换成 int time_gap() const { return time_gap_; },void set_time_gap(int v) { time_gap_=v; }, time_gap_ {500};
    DEF_PARAM(int,time_gap,500);
    DEF_PARAM(int,vol,1);
    DEF_PARAM(int,max_holding,3);
    DEF_PARAM(int,trade_limit,20);
    DEF_PARAM(bool,delivery,false);
    DEF_PARAM(double,IF_multiplier,1.5);
    DEF_PARAM(double,IC_base,0.0);
    DEF_PARAM(double,IF_base,0.0);
    DEF_PARAM(double,IC_index_base,0.0);
    DEF_PARAM(double,IF_index_base,0.0);
    DEF_PARAM(double,short_base,5.0);
    DEF_PARAM(double,long_base,-5.0);
    DEF_PARAM(double,long_nstd,1.0);
    DEF_PARAM(double,short_nstd,1.0);
    DEF_PARAM(double,short_exit_thres,-5);
    DEF_PARAM(double,long_exit_thres,5);
    DEF_PARAM(double,ma_margin_index_return_thres,0);
    DEF_PARAM(int,window,1800);
    DEF_PARAM(int,latency,500);

    IndexArb(const Config& config) : Template(config)
    {}

    ~IndexArb()
    {
        Config config("./config.json");
        config.set_config("strategy_config",{
                                                {"IF_yst_long_pos",IF->long_pos()},
                                                {"IF_yst_short_pos",IF->short_pos()},
                                                {"IC_yst_long_pos",IC->long_pos()},
                                                {"IC_yst_short_pos",IC->short_pos()}
                                             });
        config.dump("./config.json");
    }

    double margin() const 
    {
        auto IC_mid = (IC->ask_1_price() + IC->bid_1_price()) / 2;
        auto IF_mid = (IF->ask_1_price() + IF->bid_1_price()) / 2;

        return IC_mid - IC_base_ - (IF_mid - IF_base_) * IF_multiplier_ - 
              (IC_index->last_price() - IC_index_base_) + (IF_index->last_price() - IF_index_base_) * IF_multiplier_;
    }

    double margin_index() const 
    {
        return (IC_index->last_price() - IC_index_base_) - (IF_index->last_price() - IF_index_base_) * IF_multiplier_;
    }

    double margin_index_return() const 
    {
        auto IC_mid = (IC->ask_1_price() + IC->bid_1_price()) / 2;
        return (margin_index() - indicator.margin_index_pre()) / IC_mid;
    }
    
    bool is_price_ok() const
    {
        return IF->is_price_ok() && IC->is_price_ok() && IF_index->last_price() > 0 && IC_index->last_price() > 0;
    }

    void killall() 
    {
        long_IC.kill();
        short_IC.kill();
        long_IF.kill();
        short_IF.kill();
    }

    void set_base() 
    {
        if (inited_)
            return;

        IC_base_ = (IC->bid_1_price() + IC->ask_1_price()) / 2;
        IF_base_ = (IF->bid_1_price() + IF->ask_1_price()) / 2;
        IC_index_base_ = IC_index->last_price();
        IF_index_base_ = IF_index->last_price();
        inited_ = true;
    }

    // 多IC空IF
    void open_long() 
    {
        if (IF->datetime().time() < Time(9,34,0) || IF->datetime().time() > Time(14,50,0))
            return;
        
        if (IC->net_pos() < 0 || IF->net_pos() > 0)
            return;

        if (delivery_ && IC->yst_short_pos() == 0 && IF->yst_long_pos() == 0)
            return;

        // 当前单边持仓不能超过限制或者今日总交易次数不能超过限制
        if ((IC->net_pos() >= max_holding_ && -IF->net_pos() >= max_holding_) || (IF->today_short_pos() > 0 && IC->today_long_pos() > 0 && IC->long_pos() >= trade_limit_ && IF->short_pos() >= trade_limit_))
            return;

            long_IC.send(IC->ask_1_price() + 1,vol_);
            short_IF.send(IF->bid_1_price() - 1,vol_);
    }

    // 空IC多IF
    void open_short() 
    {
        if (IF->datetime().time() < Time(9,34,0) || IF->datetime().time() > Time(14,50,0))
            return;
        
        if (IC->net_pos() > 0 || IF->net_pos() < 0)
            return;
        
        if (delivery_ && IC->yst_long_pos() == 0 && IF->yst_short_pos() == 0)
            return;
            
        if ((-IC->net_pos() >= max_holding_ && IF->net_pos() >= max_holding_) || (IF->today_long_pos() > 0 && IC->today_short_pos() > 0 && IC->short_pos() >= trade_limit_ && IF->long_pos() >= trade_limit_))
            return;

        short_IC.send(IC->bid_1_price() - 1 ,vol_);
        long_IF.send(IF->ask_1_price() + 1,vol_);
    }

    // 平仓，多IC空IF,只平1手，成交后在on_trade里再继续发1手，直到平完。
    void close_long() 
    {
        if (IC->net_pos() > 0)
            short_IC.send(IC->bid_1_price() - 1,1);
        
        if (IF->net_pos() < 0)
            long_IF.send(IF->ask_1_price() + 1,1);
    }

    // 平仓，空IC多IF，成交后在on_order里再继续发1手，直到平完。
    void close_short() 
    {
        if (IC->net_pos() < 0)
            long_IC.send(IC->ask_1_price() + 1,1);

        if (IF->net_pos() > 0)
            short_IF.send(IF->bid_1_price() - 1,1);     
    }

    void force_close() 
    {
        if (!force_close_finish)
        {
            if (IC->net_pos() != 0 || IF->net_pos() != 0)       // 收盘强平，如果某个标的净头寸不为0,就平掉多出来的头寸(用Offset::OPEN)
            {
                if (IC->net_pos() > 0)
                {
                    short_IC.set_offset(Offset::OPEN);
                    short_IC.send(IC->bid_1_price() - 1,1);
                }

                if (IC->net_pos() < 0)
                {
                    long_IC.set_offset(Offset::OPEN);
                    long_IC.send(IC->ask_1_price() + 1,1);
                }

                if (IF->net_pos() > 0)
                {   
                    short_IF.set_offset(Offset::OPEN);
                    short_IF.send(IF->bid_1_price() - 1,1);
                }

                if (IF->net_pos() < 0)
                {
                    long_IF.set_offset(Offset::OPEN);
                    long_IF.send(IF->ask_1_price() + 1,1);
                }
            }

            force_close_finish = true;
        }
    }

    // 延迟发单的回调函数，达到一定延迟后(变量lantency_），调用这个函数
    void latency_send_order(const Event& event)
    {
        auto trade = event.data<TradeData>();
        auto order_id = trade.order_id;
        auto offset = trade.offset;
        auto smartorder = id_to_smartorder(order_id);  // 找出这个order_id是哪个smartorder发出的
        auto inst = smartorder->instrument();  // 得到这个smartorder绑定的instrument
        
        // 平仓成功，继续平1手
        if (inst->net_pos() > 0 && smartorder->side() == OrderSide::SHORT)
        {  
            smartorder->send(inst->bid_1_price() - 1,1);
        }

        else if (inst->net_pos() < 0 && smartorder->side() == OrderSide::LONG)
        {
            smartorder->send(inst->ask_1_price() + 1,1);
        }
    }

    void on_tick(const TickData& tick)
    {
        if (IF->datetime().time() < Time(9,30,0) || IF->datetime().time() > Time(14,55,0))
            return;

        if (strcmp(tick.symbol,IF->symbol()) == 0)
        {
            set_base();
            auto _margin = margin();
            indicator.update_margin(_margin);
            indicator.update_margin_index(margin_index());
            indicator.update_margin_index_return(3900 * margin_index_return());
            indicator.update_sig_list(_margin);

            if (!indicator.is_ready())
            {
                return;
            }

            auto ma_margin = indicator.ma_margin();
            auto margin_pre = indicator.margin_pre();
            auto ma_margin_index_return = indicator.ma_margin_index_return();
            auto std = indicator.std();

            auto short_thres = short_base_ + short_nstd_ * std;
            auto long_thres = long_base_ + long_nstd_ * std;

            auto short_signal = (_margin >= ma_margin + short_thres && 
                                  margin_pre >= ma_margin + short_thres &&
                                  ma_margin_index_return < ma_margin_index_return_thres_ && 
                                  (indicator.n() - indicator.short_timer() > time_gap_)
                                  );
                                  
            auto long_signal = (_margin <= ma_margin + long_thres && 
                                 margin_pre <= ma_margin + long_thres &&
                                 ma_margin_index_return > ma_margin_index_return_thres_ && 
                                 (indicator.n() - indicator.long_timer() > time_gap_)
                                 );

            auto close_long_signal = (_margin >= ma_margin + long_exit_thres_ &&
                                      margin_pre >= ma_margin + long_exit_thres_ &&
                                      ma_margin_index_return < ma_margin_index_return_thres_);

            auto close_short_signal = (_margin <= ma_margin + short_exit_thres_ &&
                                       margin_pre <= ma_margin + short_exit_thres_ &&
                                       ma_margin_index_return > ma_margin_index_return_thres_);

            if (long_signal) open_long();
            if (short_signal) open_short();
            if (close_long_signal) close_long();
            if (close_short_signal) close_short();
        }
    }

    void on_order(const OrderData& order)
    {   // 订单一发出去就尝试撤单，撤单失败就说明全部成交。
        // 如果能撤单成功，回报到来的时候会重新调用这个函数。

        killall();                                          
        if (order.status == Status::CANCELLED)  
        {         
            auto smartorder = id_to_smartorder(order.order_id);
            auto instrument = smartorder->instrument();

            if (order.side == OrderSide::LONG)
            {
                smartorder->send(instrument->ask_1_price() + 2,order.volume_left);
            }
            else
            {
                smartorder->send(instrument->bid_1_price() - 2,order.volume_left);
            }
        }
    }
    
    void on_trade(const TradeData& trade)
    {   
        add_timer(latency_,trade,std::bind(&IndexArb::latency_send_order,this,std::placeholders::_1));
        // 开仓或加仓后，记录tick数
        
        if (IC->net_pos() !=0 || IF->net_pos() != 0)
        {   // 新开仓或加仓以后设置计数器当前tick数
            if ((IC->net_pos() > 0 && (strcmp(trade.symbol,IC->symbol()) == 0) && trade.side == OrderSide::LONG) ||
                (IF->net_pos() < 0 && (strcmp(trade.symbol,IF->symbol()) == 0) && trade.side == OrderSide::SHORT))
                {
                    indicator.set_long_timer(indicator.n());   
                }

            else if ((IC->net_pos() < 0 && (strcmp(trade.symbol,IC->symbol()) == 0) && trade.side == OrderSide::SHORT) ||
                (IF->net_pos() > 0 && (strcmp(trade.symbol,IF->symbol()) == 0) && trade.side == OrderSide::LONG))
                {
                    indicator.set_short_timer(indicator.n());
                }
        }
        else if (IC->net_pos() == 0 && IF->net_pos() == 0)          // 全部平仓完以后，将计数器tick数设置为0
        {
            indicator.set_long_timer(0);
            indicator.set_short_timer(0);
        }
        // ICIF有昨仓被平完，设置为开仓指令
        if (IC->yst_short_pos() == 0 && IF->yst_long_pos() == 0 ||
            IF->yst_long_pos() == 0 && IC->yst_short_pos() == 0)
        {
            long_IC.set_offset(Offset::OPEN);
            long_IF.set_offset(Offset::OPEN);
            short_IF.set_offset(Offset::OPEN);
            short_IC.set_offset(Offset::OPEN);
        }
    }

    void init() override
    {   // 配置参数，订阅标的，设置下单对象。
        auto IC_symbol = config_.get_string("IC_symbol");
        auto IF_symbol = config_.get_string("IF_symbol");

        IC = subscribe(IC_symbol,GatewayType::CTP);
        IF = subscribe(IF_symbol,GatewayType::CTP);
        IC_index = subscribe("000905",GatewayType::L2);
        IF_index = subscribe("000300",GatewayType::L2);

        time_gap_ = config_.get_int("time_gap");
        vol_ = config_.get_int("vol");
        max_holding_ = config_.get_int("max_holding");
        trade_limit_ = config_.get_int("trade_limit");
        delivery_ = config_.get_bool("delivery");
        IF_multiplier_ = config_.get_double("IF_multiplier");
        short_base_ = config_.get_double("short_base");
        long_base_ = config_.get_double("long_base");
        short_nstd_ = config_.get_double("short_nstd");
        long_nstd_ = config_.get_double("long_nstd");
        short_exit_thres_ = config_.get_double("short_exit_thres");
        long_exit_thres_ = config_.get_double("long_exit_thres");
        ma_margin_index_return_thres_ = config_.get_double("ma_margin_index_return_thres");
        window_ = config_.get_int("window");
        latency_ = config_.get_int("latency");
       
        auto tick_size = config_.get_double("tick_size");

        IF->set_tick_size(tick_size);
        IC->set_tick_size(tick_size);

        auto IF_yst_long_pos = config_.get_int("IF_yst_long_pos");
        auto IC_yst_long_pos = config_.get_int("IC_yst_long_pos");
        auto IF_yst_short_pos = config_.get_int("IF_yst_short_pos");
        auto IC_yst_short_pos = config_.get_int("IC_yst_short_pos");

        IF->set_yst_long_pos(IF_yst_long_pos);
        IF->set_yst_short_pos(IF_yst_short_pos);
        IC->set_yst_long_pos(IC_yst_long_pos);
        IC->set_yst_short_pos(IC_yst_short_pos);

        if (IC->yst_short_pos())
            long_IC = SmartOrder(IC,OrderType::LIMIT,OrderSide::LONG,Offset::CLOSE);
        else
            long_IC = SmartOrder(IC,OrderType::LIMIT,OrderSide::LONG,Offset::OPEN); 

        if (IF->yst_short_pos())
            long_IF = SmartOrder(IF,OrderType::LIMIT,OrderSide::LONG,Offset::CLOSE);
        else
            long_IF = SmartOrder(IF,OrderType::LIMIT,OrderSide::LONG,Offset::OPEN); 
        
        if (IC->yst_long_pos())
            short_IC = SmartOrder(IC,OrderType::LIMIT,OrderSide::SHORT,Offset::CLOSE);
        else
            short_IC = SmartOrder(IC,OrderType::LIMIT,OrderSide::SHORT,Offset::OPEN); 
        
        if (IF->yst_long_pos())
            short_IF = SmartOrder(IF,OrderType::LIMIT,OrderSide::SHORT,Offset::CLOSE);
        else
            short_IF = SmartOrder(IF,OrderType::LIMIT,OrderSide::SHORT,Offset::OPEN); 
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
        
        case EventType::EVENT_ORDER:
            on_order(event.data<OrderData>());
            break;

        case EventType::EVENT_TRADE:
            on_trade(event.data<TradeData>());
            break;
        }

        if (DateTime::now().time() > Time(14,55,15))
        {
            force_close();
        }
    }

private:
    Instrument *IC;
    Instrument *IF;
    Instrument *IC_index;
    Instrument *IF_index;

    SmartOrder long_IC;
    SmartOrder short_IC;
    SmartOrder long_IF;
    SmartOrder short_IF;

    Indicator indicator;

    bool inited_ { false };
    bool force_close_finish { false };

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
    Template* strategy_impl = new IndexArb(strategy_config); // 实例化一个策略
    engine.add_strategy(strategy_impl);   // 把策略加入(注册)到算法引擎中(相应的行情和回报会推送给策略)
    
    stop_engine = [&engine](){ engine.~AlgoEngine(); };
    
    signal(SIGINT,handler);
    signal(SIGTERM,handler);
    engine.start();  // 启动

}

