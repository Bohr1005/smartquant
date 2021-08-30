#ifndef SMARTQUANT_BACKTESTING_H
#define SMARTQUANT_BACKTESTING_H
#include <string>


namespace SmartQuant
{
    class Instrument;

    class Template;

    class SmartOrder;
    
    class BackTestingEngine
    {
    public:
        Instrument* subscribe(Template* strategy,const std::string& instrumentID);
    private:

    };


    class DailyResult
    {
    public:

    private:

    };
} // namespace SmartQuant


#endif