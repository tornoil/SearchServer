#pragma once

#include <chrono>
#include <iostream>

#define LOG_DURATION(x) LogDuration ld((x))

class LogDuration {
public:
    // заменим имя типа std::chrono::steady_clock
    // с помощью using для удобства
    using Clock = std::chrono::steady_clock;

    LogDuration() {
    }
    LogDuration(const std::string& str){
        str_ = str;
    }
    LogDuration(const std::string_view& str){
        std::string str_view_convert(str);
        str_ = str_view_convert;
    }
    ~LogDuration() {
        using namespace std::chrono;
        using namespace std::literals;

        const auto end_time = Clock::now();
        const auto dur = end_time - start_time_;
        std::cerr << str_ <<": "s << duration_cast<milliseconds>(dur).count() << " ms"s << std::endl;
    }

private:
    const Clock::time_point start_time_ = Clock::now();
    std::string str_;
};