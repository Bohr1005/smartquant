#ifndef SMARTQUANT_BARGENERATOR_H
#define SMARTQUANT_BARGENERATOR_H
#include "declare.h"
#include "object.h"
#include <functional>

namespace SmartQuant
{
    class BarGenerator
    {
    public:
        using Callable = std::function<void(const BarData&)>;
        
        BarGenerator() = delete;

        BarGenerator(Callable on_bar,Callable on_window_bar,const Frequency& frequency,int window);

        void update_tick(const TickData& tick);

        void update_bar(const BarData& bar);

    private:
        Callable on_bar_;
        
        Callable on_window_bar_;

        Frequency frequency_;

        int window_;

        BarData cur_bar_;

        BarData last_bar_;

        TickData last_tick_;
    };
} // namespace SmartQuant


#endif