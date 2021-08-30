#include "object.h"

int main()
{
    using namespace SmartQuant;
    TickData tick;
    std::cout<<tick.datetime<<','
             <<tick.open_price<<','
             <<tick.high_price<<','
             <<tick.low_price<<','
             <<tick.last_price<<std::endl;
}