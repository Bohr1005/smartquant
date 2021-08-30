#include "event/include/event.h"
#include "declare.h"
#include <iostream>

SMARTQUANT_NAMESPACE_START

/////////////////////////////////////////////////////////////
EventEngine::EventEngine(int size) : queue_(size) {};

void EventEngine::push(const Event& e)
{
    queue_.try_enqueue(e);
}

void EventEngine::start()
{
    is_active_ = true;
    thread_ = std::thread(&EventEngine::run,this);
    thread_.join();
}

void EventEngine::stop()
{
    is_active_ = false;
}

void EventEngine::register_handler(const EventType& event_type,callable callback)
{
    handlers_[event_type].push_back(callback);
}

void EventEngine::run()
{
    Event event;
    while(is_active_)
    {   
        queue_.wait_dequeue(event);

        for(auto callback : handlers_[event.type()])
        {
            callback(event);
        }  
    }
}

void EventEngine::_add_timer(int latency,const std::any& data,callable callback)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(latency));
    Event event(EventType::EVENT_TIMER,data,callback);
    push(event);
}

void EventEngine::add_timer(int latency,const std::any& data,callable callback)
{
    auto thread = std::thread(&EventEngine::_add_timer,this,latency,data,callback);
    thread.detach();
}

SMARTQUANT_NAMESPACE_END
