#pragma once

namespace GameTime {
    // 基础时间单位常量
    constexpr int MINUTES_PER_HOUR = 60;
    constexpr int HOURS_PER_DAY = 24;
    constexpr int DAYS_PER_SEASON = 28;
    constexpr int SEASONS_PER_YEAR = 4;
    
    // 衍生常量
    constexpr int MINUTES_PER_DAY = MINUTES_PER_HOUR * HOURS_PER_DAY;
    constexpr int MINUTES_PER_SEASON = MINUTES_PER_DAY * DAYS_PER_SEASON;
    constexpr int MINUTES_PER_YEAR = MINUTES_PER_SEASON * SEASONS_PER_YEAR;
    
    // 时间尺度常量
    constexpr float DEFAULT_TIME_SCALE = 1.0f; 
    constexpr float MIN_TIME_SCALE = 0.1f;
    constexpr float MAX_TIME_SCALE = 1000.0f;
    
    // 默认起始时间
    constexpr int DEFAULT_YEAR = 2024;
    constexpr int DEFAULT_SEASON = 0;  // 春季
    constexpr int DEFAULT_DAY = 1;
    constexpr int DEFAULT_HOUR = 0;
    constexpr int DEFAULT_MINUTE = 0;
}
