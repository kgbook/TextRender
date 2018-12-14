//
// Created by kang on 2018/12/14.
//

#include <chrono>
#include "time.h"
#include "exception.h"

using namespace std::chrono;

Time::Time(TimeZone tz)
    :time_zone_(tz){
    //
}

Time* Time::getInstance() {
    static Time time(TimeZone::UTC08);
    return &time;
}

time_t Time::getTimestamp() {
    system_clock::time_point tp = system_clock::now();
    return system_clock::to_time_t(tp);
}

std::string Time::getTimeString(TimeFormat time_format) {
    std::string time_str;
    auto utc_tm = getTm();
    char buf[64];

    switch (time_format) {
        case TimeFormat::YYYYMMDD_hhmmss : {
            snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", utc_tm->tm_year + 1900, utc_tm->tm_mon + 1,
                     utc_tm->tm_mday, utc_tm->tm_hour, utc_tm->tm_min, utc_tm->tm_sec);

            time_str = std::string(buf);
            break;
        }

        default: {
            Exception::runtimeAssert("Not support such time format!", Exception::ErrorCode::notSupport);
        }
    }
    return time_str;
}

std::shared_ptr<std::tm> Time::getTm() {
    auto ts = getTimestamp();
    std::tm utc_tm;

    switch (time_zone_) {
        case TimeZone::UTC08 : {
            ts += 8 * 60 * 60;
            break;
        }

        default: {
            Exception::runtimeAssert("Not support such time zone!", Exception::ErrorCode::notSupport);
        }
    }

    if (nullptr != gmtime_r(&ts, &utc_tm)) {
        std::shared_ptr<tm> tm_ptr = std::make_shared<tm>();
        *tm_ptr = utc_tm;
        return tm_ptr;
    }
    return nullptr;
}
