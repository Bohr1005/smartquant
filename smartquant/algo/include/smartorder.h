#ifndef SMARTQUANT_SMARTQUANT_H
#define SMARTQUANT_SMARTQUANT_H
#include "object.h"
#include <memory>

namespace SmartQuant
{
    class AlgoEngine;

    class Instrument;

    class SmartOrder
    {
    public:
        SmartOrder() = default;

        SmartOrder(const SmartOrder& smartorder) = delete;

        SmartOrder(Instrument* instrument,const OrderType& type,const OrderSide& side,const Offset& offset);

        SmartOrder& operator=(const SmartOrder&& smartorder);

        Instrument* instrument() const;

        void set_type(const OrderType& type);

        void set_side(const OrderSide& side);

        void set_offset(const Offset& offset);

        int64_t order_id() const;

        double price() const;

        int volume() const;

        OrderSide side() const;

        bool is_active() const;

        void send(const double& price,const int& volume);

        void send(const double& price,const int& volume,const OrderType& type,const Offset& offset);

        void kill();

    private:
        friend class AlgoEngine;
        
        Instrument* instrument_;

        AlgoEngine* engine_;

        int64_t order_id_;

        OrderType type_;
        OrderSide side_;
        Offset offset_;

        double price_;
        int volume_;
        
        Status status_;
        
        bool is_active_;
    };
} // namespace SmartQuant


#endif