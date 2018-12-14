//
// Created by kang on 2018/12/14.
//

#ifndef OWL_TIME_H
#define OWL_TIME_H

#include <cstdint>
#include <string>
#include <ctime>

enum class TimeFormat {
    YYYYMMDD_hhmmss,

    ksize    /* the size of the enum, sometimes also indicate an error if return value is DataFormat::size */
};

enum class TimeZone {
    UTC08,

    kSize
};

class Time {
public:
    Time(TimeZone tz = TimeZone::UTC08);

    ~Time() = default;

    Time& operator = (Time &) = delete;

    Time(Time &) = delete;

    static Time *getInstance();

    std::string getTimeString(TimeFormat time_format = TimeFormat::YYYYMMDD_hhmmss);

private:
    std::shared_ptr<std::tm> getTm();

    time_t getTimestamp();

    TimeZone time_zone_;
};


#endif //OWL_TIME_H
