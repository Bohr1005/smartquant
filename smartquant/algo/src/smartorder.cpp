#include "algo/include/smartorder.h"
#include "declare.h"
#include "algo/include/instrument.h"
#include "algo/include/algo.h"
#include <algorithm>
#include <iostream>
SMARTQUANT_NAMESPACE_START

SmartOrder::SmartOrder(Instrument* instrument,const OrderType& type,const OrderSide& side,const Offset& offset)
: instrument_(instrument), engine_(instrument->engine()),order_id_(0),
  type_(type),side_(side),offset_(offset),
  price_(0),volume_(0),status_(Status::UNKOWN),is_active_(false)
{}

SmartOrder& SmartOrder::operator=(const SmartOrder&& smartorder)
{
    if (this == &smartorder)
        return *this;

    instrument_ = smartorder.instrument_;
    engine_ = smartorder.engine_;
    order_id_ = smartorder.order_id_;
    type_ = smartorder.type_;
    side_ = smartorder.side_;
    offset_ = smartorder.offset_;
    price_ = smartorder.price_;
    volume_ = smartorder.volume_;
    is_active_ = smartorder.is_active_;

    return *this;
}

Instrument* SmartOrder::instrument() const
{
    return instrument_;
}

void SmartOrder::set_type(const OrderType& type)
{
    type_ = type;
}

void SmartOrder::set_side(const OrderSide& side)
{
    side_ = side;
}

void SmartOrder::set_offset(const Offset& offset)
{
    offset_ = offset;
}

int64_t SmartOrder::order_id() const
{
    return order_id_;
}

double SmartOrder::price() const
{
    return price_;
}

int SmartOrder::volume() const
{
    return volume_;
}

OrderSide SmartOrder::side() const
{
    return side_;
}

bool SmartOrder::is_active() const
{
    return is_active_;
}

void SmartOrder::send(const double& price,const int& volume)
{
    if (is_active_ || !instrument_->tradable_)
        return;

    if (price <= 0 || volume <= 0)
        return;

    OrderRequest req {};
    strcpy(req.symbol,instrument_->symbol());
    strcpy(req.exchange,instrument_->exchange());
    req.gateway = instrument_->gateway();
    req.type = type_;
    req.side = side_;
    req.price = price;
    req.volume = volume;

    price_ = price;
    volume_ = volume;

    (req.side == OrderSide::LONG)? ++instrument_->long_times_ : ++instrument_->short_times_;

    if (offset_ == Offset::AUTO)
    {
        switch (side_)
        {
        case OrderSide::LONG:
            instrument_->short_pos() > 0 ? req.offset = Offset::CLOSETODAY : req.offset = Offset::OPEN;
            break;
        
        case OrderSide::SHORT:
            instrument_->long_pos() > 0 ? req.offset = Offset::CLOSETODAY : req.offset = Offset::OPEN;
            break;
        }
    }
    else
    {
        req.offset = offset_;
    }
    
    order_id_ = engine_->send_order(req);
    is_active_ = true;
    engine_->id_to_smartorder_[order_id_] = this;
}

void SmartOrder::send(const double& price,const int& volume,const OrderType& type,const Offset& offset)
{
    if (is_active_ || !instrument_->tradable_)
        return;

    if (price <= 0 || volume <= 0)
        return;

    OrderRequest req {};
    strcpy(req.symbol,instrument_->symbol());
    strcpy(req.exchange,instrument_->exchange());
    req.gateway = instrument_->gateway();
    req.type = type;
    req.side = side_;
    req.price = price;
    req.volume = volume;

    price_ = price;
    volume_ = volume;

    (req.side == OrderSide::LONG)? ++instrument_->long_times_ : ++instrument_->short_times_;

    if (offset == Offset::AUTO)
    {
        switch (side_)
        {
        case OrderSide::LONG:
            instrument_->short_pos() > 0 ? req.offset = Offset::CLOSETODAY : req.offset = Offset::OPEN;
            break;
        
        case OrderSide::SHORT:
            instrument_->long_pos() > 0 ? req.offset = Offset::CLOSETODAY : req.offset = Offset::OPEN;
            break;
        }
    }
    else
    {
        req.offset = offset;
    }

    order_id_ = engine_->send_order(req);
    is_active_ = true;
    engine_->id_to_smartorder_[order_id_] = this;
}

void SmartOrder::kill()
{
    if (!is_active_)
    {
        return;
    }
    
    CancelRequest req {};
    strcpy(req.symbol,instrument_->symbol());
    strcpy(req.exchange,instrument_->exchange());
    req.gateway = instrument_->gateway();
    req.order_ref = order_id_;

    engine_->cancel_order(req);
}

SMARTQUANT_NAMESPACE_END