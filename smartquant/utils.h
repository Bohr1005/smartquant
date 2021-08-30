#ifndef SMARTQUANT_UTILS_H
#define SMARTQUANT_UTILS_H

#include <string>
#include <fstream>
#include <vector>
#include <cassert>
#include <chrono>
#include <iomanip>
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include <iostream>
#include <ctime>

namespace SmartQuant
{
    enum class GatewayType : u_int32_t;

    spdlog::level::level_enum get_log_level(const std::string& level);

    std::string to_string(const GatewayType& gateway_type);

    GatewayType from_string(std::string gateway_type);

/////////////////////////////////////////////////////////////

    class key_error : public std::runtime_error
    {
    public:
        key_error(const std::string& msg) : std::runtime_error(msg)
        {}
    };

    class Config
    {
    public:
        Config(std::string path);

        Config(const nlohmann::json::initializer_list_t& l);

        Config(const std::string& key,const nlohmann::json& value);
        
        std::string name() const;

        std::vector<Config> children();

        bool contain(const std::string& key);
        
        std::string get_string(const std::string& key);

        int get_int(const std::string& key);

        double get_double(const std::string& key);

        bool get_bool(const std::string& key);

        Config get_config(const std::string& key);

        template<typename T>
        void set_value(const std::string& key,const T& v)
        {
            j_[key] = v;
        }

        void set_config(const std::string& key,const nlohmann::json::initializer_list_t& v);

        void dump(const std::string& path) const;

    public:
        std::string key_;
        nlohmann::json j_;
    };

/////////////////////////////////////////////////////////////

    using days = std::chrono::duration
        <int, std::ratio_multiply<std::ratio<24>, std::chrono::hours::period>>;

    using years = std::chrono::duration
        <int, std::ratio_multiply<std::ratio<146097, 400>, days::period>>;

    using months = std::chrono::duration
        <int, std::ratio_divide<years::period, std::ratio<12>>>;

    using hours = std::chrono::hours;

    using minutes = std::chrono::minutes;

    using seconds = std::chrono::seconds;

    using milliseconds = std::chrono::milliseconds;

    class Date
    {
    public:
        Date();

        Date( int year, int month, int day);

        int year() const;

        int month() const;

        int day() const;

    private:
        int year_;
        
        int month_;
        
        int day_;
    };

    bool operator==(const Date& x, const Date& y);

    bool operator!=(const Date& x, const Date& y);

    bool operator<(const Date& x, const Date& y);

    bool operator<=(const Date& x, const Date& y);

    bool operator>(const Date& x, const Date& y);

    bool operator>=(const Date& x, const Date& y);

    std::ostream & operator<<(std::ostream &os, Date &date);

/////////////////////////////////////////////////////////////

    class Time
    {
    public:
        Time();

        Time(int hour,int minute,int second);

        Time(int hour,int minute,int second,int millisecond);

        int hour() const;

        int minute() const;

        int second() const;
        
        int millisecond() const;
    private:
        int hour_;
    
        int minute_;
    
        int second_;
    
        int millisecond_;
    };

    bool operator==(const Time&x, const Time& y);

    bool operator!=(const Time& x, const Time& y);

    bool operator<(const Time& x, const Time& y);

    bool operator<=(const Time& x, const Time& y);

    bool operator>(const Time& x, const Time& y);

    bool operator>=(const Time& x, const Time& y);

    std::ostream & operator<<(std::ostream &os, Time &time);

/////////////////////////////////////////////////////////////

    class DateTime
    {
    public:
        DateTime() = default;

        DateTime(int year,int month,int day );
        
        DateTime(int year,int month,int day,int hour,int minute,int second );
        
        DateTime(int year,int month,int day,int hour,int minute,int second,int millisecond );

        const Date& date() const;

        const Time& time() const;

        static DateTime now();
        
    private:
        friend std::ostream & operator<<(std::ostream &os, DateTime &datetime);

        friend std::ostream & operator<<(std::ostream &os, DateTime &&datetime);

        Date date_;
    
        Time time_;
    };

    bool operator<(const DateTime&  x, const DateTime& y);

    bool operator>(const DateTime&  x, const DateTime& y);

    bool operator<=(const DateTime&  x, const DateTime& y);

    bool operator>=(const DateTime&  x, const DateTime& y);

    bool operator==(const DateTime&  x, const DateTime& y);

    bool operator!=(const DateTime&  x, const DateTime& y);

    std::ostream & operator<<(std::ostream &os, DateTime &datetime);

    std::ostream & operator<<(std::ostream &os, DateTime &&datetime);
} // namespace SmartQuant

#endif