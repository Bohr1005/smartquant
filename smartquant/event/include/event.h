#ifndef SMARTQUANT_EVENT_H
#define SMARTQUANT_EVENT_H
#include <string>
#include <unordered_map>
#include <thread>
#include <functional>
#include <vector>
#include <any>
#include "concurrentqueue/blockingconcurrentqueue.h"

namespace SmartQuant
{
 
    enum class EventType : uint32_t
    {
        EVENT_TICK,
        EVENT_TRADE,
        EVENT_ORDER,
        EVENT_TIMER
    };

/////////////////////////////////////////////////////////////

    class Event
    {
    public:
        using callable = std::function<void(const Event&)>;

        Event() = default;

        Event(const Event& event)
        {
            type_ = event.type_;
            data_ = event.data_;
            callback_ = event.callback_;
        }

        Event(const EventType& type,const std::any& data) : type_(type),data_(data) {}

        Event(const EventType& type,const std::any& data,callable callback) : type_(type),data_(data),callback_(callback) {}

        const EventType& type() const
        { return type_; }

        template<typename T>
        const T& data() const
        {
            return std::any_cast<const T &>(data_);
        }   

        void invoke_callback() const
        {
            callback_(*this);
        }

    private:
        EventType type_;
        
        std::any data_;

        callable callback_;
    };

/////////////////////////////////////////////////////////////

    class EventEngine
    {
    public:
        using callable = std::function<void(const Event&)>;

        EventEngine(int size);

        void push(const Event& event);

        void start();

        void stop();

        void register_handler(const EventType& event_type,callable callback);

        void run();

        void add_timer(int latency,const std::any& data,callable callback);

    private:
        moodycamel::BlockingConcurrentQueue<Event> queue_;

        std::thread thread_;

        std::unordered_map<EventType,std::vector<callable>> handlers_;

        bool is_active_;

        void _add_timer(int latency,const std::any& data,callable callback);
    };
}; // namespace SmartQuant

#endif