#include "declare.h"
#include "object.h"
#include <sys/time.h>

SMARTQUANT_NAMESPACE_START

std::string to_string(const GatewayType& gateway_type)
{
    switch (gateway_type)
    {
    case GatewayType::CTP:
        return "CTP";
    case GatewayType::UNKNOW:
        return "KNOWN";
    default:
        return "KNOWN";
    }
}

GatewayType from_string(std::string gateway_type)
{
    if (gateway_type == "CTP")
        return GatewayType::CTP;
    else
        return GatewayType::UNKNOW;
}

spdlog::level::level_enum get_log_level(const std::string& level)
{
    if (level == "trace")
        return spdlog::level::trace;

    else if (level == "debug")
        return spdlog::level::debug;

    else if (level == "info")
        return spdlog::level::info;
    else if (level == "warn")
        return spdlog::level::warn;

    else if (level == "err")
        return spdlog::level::err;

    else if (level == "critical")
        return spdlog::level::critical;

    else if (level == "off")
        return spdlog::level::off;

    else
        return spdlog::level::info;
}

Config::Config(std::string path)
{
    std::ifstream ifs(path);
    ifs>>j_;
    ifs.close();
}

Config::Config(const nlohmann::json::initializer_list_t& l)
{
    j_ = l;
    std::cout<<1<<std::endl;
}

Config::Config(const std::string& key,const nlohmann::json& value) : key_(key),j_(value) {}

std::string Config::name() const
{ return key_; }

std::vector<Config> Config::children()
{
    std::vector<Config> vec_;
    for (auto& item : j_.items())
    {
        if (item.value().is_object())
        {
            Config config(item.key(),item.value());
            vec_.push_back(config);
        }
    }
    return vec_;
}

bool Config::contain(const std::string& key)
{
    return j_.contains(key);
}

std::string Config::get_string(const std::string& key)
{ 
    if (!j_.contains(key))
    {
        throw key_error("KeyError:" + key);
    }
    return j_[key].get<std::string>(); 
}

int Config::get_int(const std::string& key)
{ 
    if (!j_.contains(key))
    {
        throw key_error("KeyError:" + key);
    }
    return j_[key].get<int>(); 
}

double Config::get_double(const std::string& key)
{ 
    if (!j_.contains(key))
    {
        throw key_error("KeyError:" + key);
    }
    return j_[key].get<double>(); 
}

bool Config::get_bool(const std::string& key)
{ 
    if (!j_.contains(key))
    {
        throw key_error("KeyError:" + key);
    }
    return j_[key].get<bool>(); 
}

Config Config::get_config(const std::string& key)
{
    if (!j_.contains(key))
    {
        throw key_error("KeyError:" + key);
    }
    return Config(key,j_[key]);
}

void Config::set_config(const std::string& key,const nlohmann::json::initializer_list_t& v)
{
    nlohmann::json temp_ = v;
    for (auto& el : temp_.items())
    {
        j_[key][el.key()] = el.value();
    }
}

void Config::dump(const std::string& path) const
{
    std::ofstream o(path);
    o<<std::setw(4)<<j_<<std::endl;
    o.close();
}

Date::Date() : year_(1970),month_(1),day_(1)
{}

Date::Date( int year, int month, int day) : year_(year),month_(month),day_(day)
{}

int Date::year() const
{
    return year_;
}

int Date::month() const
{
    return month_;
}

int Date::day() const
{
    return day_;
}

bool operator==(const Date& x, const Date& y)
{
    return x.year()==y.year() && x.month()==y.month() && x.day()==y.day();
}

bool operator!=(const Date& x, const Date& y)
{
    return !(x==y);
}

bool operator<=(const Date& x, const Date& y)
{
    return std::chrono::duration_cast<days>(years(x.year()) + months(x.month()) + days(x.day())) 
    <= std::chrono::duration_cast<days>(years(y.year()) + months(y.month()) + days(y.day()));
}

bool operator<(const Date& x, const Date& y)
{
    return std::chrono::duration_cast<days>(years(x.year()) + months(x.month()) + days(x.day())) 
    < std::chrono::duration_cast<days>(years(y.year()) + months(y.month()) + days(y.day()));
}

bool operator>=(const Date& x, const Date& y)
{
    return std::chrono::duration_cast<days>(years(x.year()) + months(x.month()) + days(x.day())) 
    >= std::chrono::duration_cast<days>(years(y.year()) + months(y.month()) + days(y.day()));
}

bool operator>(const Date& x, const Date& y)
{
    return std::chrono::duration_cast<days>(years(x.year()) + months(x.month()) + days(x.day())) 
    > std::chrono::duration_cast<days>(years(y.year()) + months(y.month()) + days(y.day()));
}

std::ostream & operator<<(std::ostream &os, Date &date)
{
    os<<std::setw(2) 
      <<std::setfill('0')
      <<date.year()
      <<'-'
      <<std::setw(2) 
      <<std::setfill('0')
      <<date.month()
      <<'-'
      <<std::setw(2) 
      <<std::setfill('0')
      <<date.day();
      return os;
}

Time::Time() : hour_(0),minute_(0),second_(0),millisecond_(0)
{}

Time::Time(int hour,int minute,int second) : hour_(hour),minute_(minute),second_(second),millisecond_(0)
{}

Time::Time(int hour,int minute,int second,int millisecond) : hour_(hour),minute_(minute),second_(second),millisecond_(millisecond)
{}

int Time::hour() const
{
    return hour_;
}

int Time::minute() const
{
    return minute_;
}

int Time::second() const
{
    return second_;
}

int Time::millisecond() const
{
    return millisecond_;
}

bool operator==(const Time&x, const Time& y)
{
    return x.hour()==y.hour() && x.minute()==y.minute() && x.second()==y.second() && x.millisecond()==y.millisecond();
}

bool operator!=(const Time& x, const Time& y)
{
    return !(x==y);
}

bool operator<(const Time& x, const Time& y)
{
    return std::chrono::duration_cast<milliseconds>(hours(x.hour()) + minutes(x.minute()) + seconds(x.second()) + milliseconds(x.millisecond())) 
    < std::chrono::duration_cast<milliseconds>(hours(y.hour()) + minutes(y.minute()) + seconds(y.second()) + milliseconds(y.millisecond()));
}

bool operator<=(const Time& x, const Time& y)
{
    return std::chrono::duration_cast<milliseconds>(hours(x.hour()) + minutes(x.minute()) + seconds(x.second()) + milliseconds(x.millisecond())) 
    <= std::chrono::duration_cast<milliseconds>(hours(y.hour()) + minutes(y.minute()) + seconds(y.second()) + milliseconds(y.millisecond()));
}

bool operator>(const Time& x, const Time& y)
{
    return std::chrono::duration_cast<milliseconds>(hours(x.hour()) + minutes(x.minute()) + seconds(x.second()) + milliseconds(x.millisecond())) 
    > std::chrono::duration_cast<milliseconds>(hours(y.hour()) + minutes(y.minute()) + seconds(y.second()) + milliseconds(y.millisecond())); 
}

bool operator>=(const Time& x, const Time& y)
{
    return std::chrono::duration_cast<milliseconds>(hours(x.hour()) + minutes(x.minute()) + seconds(x.second()) + milliseconds(x.millisecond())) 
    >= std::chrono::duration_cast<milliseconds>(hours(y.hour()) + minutes(y.minute()) + seconds(y.second()) + milliseconds(y.millisecond())); 
}

std::ostream & operator<<(std::ostream &os, Time &time)
{
    os<<std::setw(2) 
      <<std::setfill('0')
      <<time.hour()
      <<":"
      <<std::setw(2) 
      <<std::setfill('0')
      <<time.minute()
      <<":"
      <<std::setw(2) 
      <<std::setfill('0')
      <<time.second()
      <<"."
      <<std::setw(3) 
      <<std::setfill('0')
      <<time.millisecond();
    return os;
}

DateTime::DateTime(int year,int month,int day ) : date_(year,month,day),time_(0,0,0,0)
{}

DateTime::DateTime(int year,int month,int day,int hour,int minute,int second ) : date_(year,month,day),time_(hour,minute,second)
{}

DateTime::DateTime(int year,int month,int day,int hour,int minute,int second,int millisecond ) : date_(year,month,day),time_(hour,minute,second,millisecond)
{}

const Date& DateTime::date() const
{
    return date_;
}

const Time& DateTime::time() const
{
    return time_;
}

DateTime DateTime::now()
{
    timeval tv;
    gettimeofday(&tv,0);
    auto tm = localtime(&tv.tv_sec);
    return DateTime(tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec,tv.tv_usec/1000);
}

bool operator<(const DateTime&  x, const DateTime& y)
{
    return std::chrono::duration_cast<milliseconds>(years(x.date().year()) + months(x.date().month()) + days(x.date().day()) + 
                                                    hours(x.time().hour()) + minutes(x.time().minute()) + seconds(x.time().second()) + milliseconds(x.time().millisecond()))
           < std::chrono::duration_cast<milliseconds>(years(y.date().year()) + months(y.date().month()) + days(y.date().day()) + 
                                                    hours(y.time().hour()) + minutes(y.time().minute()) + seconds(y.time().second()) + milliseconds(y.time().millisecond()));
}

bool operator<=(const DateTime&  x, const DateTime& y)
{
    return std::chrono::duration_cast<milliseconds>(years(x.date().year()) + months(x.date().month()) + days(x.date().day()) + 
                                                    hours(x.time().hour()) + minutes(x.time().minute()) + seconds(x.time().second()) + milliseconds(x.time().millisecond()))
           <= std::chrono::duration_cast<milliseconds>(years(y.date().year()) + months(y.date().month()) + days(y.date().day()) + 
                                                    hours(y.time().hour()) + minutes(y.time().minute()) + seconds(y.time().second()) + milliseconds(y.time().millisecond()));
}

bool operator>(const DateTime&  x, const DateTime& y)
{
    return std::chrono::duration_cast<milliseconds>(years(x.date().year()) + months(x.date().month()) + days(x.date().day()) + 
                                                    hours(x.time().hour()) + minutes(x.time().minute()) + seconds(x.time().second()) + milliseconds(x.time().millisecond()))
           > std::chrono::duration_cast<milliseconds>(years(y.date().year()) + months(y.date().month()) + days(y.date().day()) + 
                                                    hours(y.time().hour()) + minutes(y.time().minute()) + seconds(y.time().second()) + milliseconds(y.time().millisecond()));
}

bool operator>=(const DateTime&  x, const DateTime& y)
{
    return std::chrono::duration_cast<milliseconds>(years(x.date().year()) + months(x.date().month()) + days(x.date().day()) + 
                                                    hours(x.time().hour()) + minutes(x.time().minute()) + seconds(x.time().second()) + milliseconds(x.time().millisecond()))
           >= std::chrono::duration_cast<milliseconds>(years(y.date().year()) + months(y.date().month()) + days(y.date().day()) + 
                                                    hours(y.time().hour()) + minutes(y.time().minute()) + seconds(y.time().second()) + milliseconds(y.time().millisecond()));
}

bool operator==(const DateTime&  x, const DateTime& y)
{
    return x.date()==y.date() && x.time()==y.time();
}

bool operator!=(const DateTime&  x, const DateTime& y)
{
    return !(x==y);
}

std::ostream & operator<<(std::ostream &os, DateTime &datetime)
{
    os<<datetime.date_<<" "<<datetime.time_;
    return os;
}

std::ostream & operator<<(std::ostream &os, DateTime &&datetime)
{
    os<<datetime.date_<<" "<<datetime.time_;
    return os;
}

SMARTQUANT_NAMESPACE_END
