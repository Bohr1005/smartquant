#include "algo/include/bargenerator.h"

SMARTQUANT_NAMESPACE_START

BarGenerator::BarGenerator(Callable on_bar,Callable on_window_bar,const Frequency& frequency,int window) 
: on_bar_(on_bar),on_window_bar_(on_window_bar),frequency_(frequency),window_(window) {}

void BarGenerator::update_tick(const TickData& tick)
{
    if (tick.last_price < 1e-10)
        return;

    if (tick.datetime < last_tick_.datetime)
        return;
}

void BarGenerator::update_bar(const BarData& bar)
{

}

SMARTQUANT_NAMESPACE_END