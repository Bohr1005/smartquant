#ifndef SMARTQUANT_TEMPLATE_H
#define SMARTQUANT_TEMPLATE_H
#include "utils.h"
#include "event/include/event.h"
#include "object.h"

#define DEF_PARAM(TYPE,NAME,VAL)                \
    private:                                    \
        TYPE NAME##_ { VAL };                   \
                                                \
    public:                                     \
        TYPE NAME() const { return NAME##_; }   \
        void set_##NAME(TYPE v)                 \
        {                                       \
            NAME##_ = v;                        \
        }                                       \


namespace SmartQuant
{
    class AlgoEngine;
    
    class Instrument;

    class SmartOrder;

    class Template
    {
    public:
        Template(const Config& config);

        virtual ~Template();

        virtual void init() = 0;

        virtual void on_event(const Event& event) = 0;
    
    protected:
        friend class Instrument;
        
        friend class AlgoEngine;

        void set_engine(AlgoEngine* engine);

        Instrument* subscribe(const std::string& symbol,const GatewayType& type);

        Instrument* subscribe(const std::string& symbol,InstrumentType instrument_type,GatewayType type);

        SmartOrder* id_to_smartorder(int64_t order_id);

        void add_timer(int latency,const std::any& data,EventEngine::callable callback);

        AlgoEngine* engine_;

        Config config_;
    };
} // namespace SmartQuant


#endif