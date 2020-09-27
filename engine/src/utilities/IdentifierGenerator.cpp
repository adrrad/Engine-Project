#include "utilities/IdentifierGenerator.hpp"
#include "utilities/Clock.hpp"
#include <iostream>
#include <chrono>
#include <ctime>

namespace Engine::Utilities
{
Clock clock;
auto start = std::chrono::steady_clock::now();
uint64_t GenerateTimeBasedID()
{
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    
    auto time = std::localtime(&now);
    int year = time->tm_year;
    int day = time->tm_yday;
    int hour = time->tm_hour;
    int minute = time->tm_min;
    int second = time->tm_sec;
    auto end = std::chrono::steady_clock::now();
    auto diff_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    return year + day + hour + minute + second + diff_ms;
}

} // namespace Engine::Utilities
