#include "algo/include/instrument.h"
#include "declare.h"
#include "algo/include/template.h"
#include "algo/include/algo.h"

SMARTQUANT_NAMESPACE_START

Instrument::Instrument(Template* strategy_impl,const std::string& symbol) 
: symbol_(symbol),strategy_impl_(strategy_impl),today_long_pos_(0),
  today_short_pos_(0),yst_long_pos_(0),yst_short_pos_(0),long_times_(0),
  short_times_(0),kill_times_(0),tick_size_(0),tradable_(true)
{};

char* Instrument::symbol() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].symbol;
}

char* Instrument::exchange() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].exchange;
}

TickData& Instrument::tickdata() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_];
}

GatewayType Instrument::gateway() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].gateway;
}

DateTime Instrument::datetime() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].datetime;
}

double Instrument::pre_close() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].pre_close_price;
}

double Instrument::pre_settlement() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].pre_settlement_price;
}

double Instrument::last_price() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].last_price;
}

int64_t Instrument::volume() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].volume;
}

double Instrument::turnover() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].turnover;
}

double Instrument::open_price() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].open_price;
}

double Instrument::high_price() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].high_price;
}

double Instrument::low_price() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].low_price;
}

double Instrument::upper_limit_price() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].upper_limit_price;
}

double Instrument::lower_limit_price() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].lower_limit_price;
}

double Instrument::bid_1_price() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].bid_1_price;
}

double Instrument::ask_1_price() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].ask_1_price;
}

double Instrument::bid_1_volume() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].bid_1_volume;
}

double Instrument::ask_1_volume() const
{
    return strategy_impl_->engine_->symbol_to_tick()[symbol_].ask_1_volume;
}

double Instrument::tick_up(double price,int n) const
{
    return price + n * tick_size_;
}

double Instrument::tick_dn(double price,int n) const
{
    return price - n * tick_size_;
}

int Instrument::to_tick_count(double price) const
{
    return price / tick_size_;
}

int Instrument::today_long_pos() const
{
    return today_long_pos_;
}

int Instrument::today_short_pos() const
{
    return today_short_pos_;
}

int Instrument::yst_long_pos() const
{
    return yst_long_pos_;
}

int Instrument::yst_short_pos() const
{
    return yst_short_pos_;
}

int Instrument::today_net_pos() const
{
    return today_long_pos_ - today_short_pos_;
}

int Instrument::yst_net_pos() const
{
    return yst_long_pos_ - yst_short_pos_;
}

int Instrument::net_pos() const
{
    return today_long_pos_ + yst_long_pos_ - today_short_pos_ - yst_short_pos_;
}

int Instrument::long_pos() const
{
    return today_long_pos_ + yst_long_pos_;
}

int Instrument::short_pos() const
{
    return today_short_pos_ + yst_short_pos_;
}

int Instrument::long_times() const
{
    return long_times_;
}

int Instrument::short_times() const
{
    return short_times_;
}

int Instrument::total_trade_times() const
{
    return long_times_ + short_times_;
}

int Instrument::kill_times() const
{
    return kill_times_;
}

void Instrument::set_kill_times(int v)
{
    kill_times_ = v;
}

void Instrument::set_tick_size(double v)
{
    tick_size_ = v;
}

void Instrument::set_today_long_pos(int v)
{
    today_long_pos_ = v;
}

void Instrument::set_today_short_pos(int v)
{
    today_short_pos_ = v;
}

void Instrument::set_yst_long_pos(int v)
{
    yst_long_pos_ = v;
}

void Instrument::set_yst_short_pos(int v)
{
    yst_short_pos_ = v;
}

void Instrument::set_tradable(bool v)
{
    tradable_ = v;
}

bool Instrument::is_price_ok() const
{
    return last_price() > 0 && ask_1_price() > 0 && bid_1_price() > 0;
}

AlgoEngine* Instrument::engine()
{
    return strategy_impl_->engine_;
}

SMARTQUANT_NAMESPACE_END