#ifndef SMARTQUANT_INSTRUMENT_H
#define SMARTQUANT_INSTRUMENT_H
#include "object.h"
#include <memory>

namespace SmartQuant
{
    class Template;

    class AlgoEngine;

    class Instrument
    {
    public:
        Instrument(Template* strategy_impl,const std::string& symbol);

        char* symbol() const;

        char* exchange() const;

        TickData& tickdata() const;

        DateTime datetime() const;

        GatewayType gateway() const;

        double pre_close() const;

        double pre_settlement() const;

        double last_price() const;

        int64_t volume() const;

        double turnover() const;

        double open_price() const;

        double high_price() const;

        double low_price() const;

        double upper_limit_price() const;

        double lower_limit_price() const;

        double bid_1_price() const;

        double ask_1_price() const;

        double bid_1_volume() const;

        double ask_1_volume() const;

        double tick_up(double price,int n) const;

        double tick_dn(double price,int n) const;

        int to_tick_count(double price) const;

        int today_long_pos() const;

        int today_short_pos() const;

        int yst_long_pos() const;

        int yst_short_pos() const;

        int today_net_pos() const;

        int yst_net_pos() const;

        int net_pos() const;

        int long_pos() const;

        int short_pos() const;

        int long_times() const;

        int short_times() const;

        int total_trade_times() const;
        
        int kill_times() const;

        void set_kill_times(int v);

        bool is_price_ok() const;

        void set_today_long_pos(int v);

        void set_today_short_pos(int v);

        void set_yst_long_pos(int v);

        void set_yst_short_pos(int v);

        void set_tick_size(double v);

        void set_tradable(bool v);
        
        AlgoEngine* engine();

    private:
        friend class AlgoEngine;

        friend class SmartOrder;

        std::string symbol_;

        Template* strategy_impl_;

        int today_long_pos_;
        int today_short_pos_;

        int yst_long_pos_;
        int yst_short_pos_;

        int long_times_;
        int short_times_;
        
        int kill_times_;

        double tick_size_;

        bool tradable_;
    };
} // namespace SmartQuant


#endif