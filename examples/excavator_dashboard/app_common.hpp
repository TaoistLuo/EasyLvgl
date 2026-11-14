#pragma once

#include "easylvgl/lvgl_widgets.hpp"
#include "i18n.hpp"

#include <cstdio>
#include <string>
#include <vector>

namespace app {

using namespace easylvgl;

constexpr uint32_t kPageBg = 0x121820;
constexpr uint32_t kGreen = 0x28C76F;
constexpr uint32_t kAmber = 0xFFB300;
constexpr uint32_t kCyan = 0x00D1FF;
constexpr uint32_t kOffBg = 0x2A3542;

inline int percent_of(int value, int maximum) {
    if (maximum <= 0) return 0;
    int p = value * 100 / maximum;
    return p < 0 ? 0 : (p > 100 ? 100 : p);
}

inline void format_uptime(uint32_t ms, char* buf, size_t size) {
    uint32_t s = ms / 1000u;
    std::snprintf(buf, size, "%02u:%02u:%02u",
                  static_cast<unsigned>(s / 3600u % 100u),
                  static_cast<unsigned>(s / 60u % 60u),
                  static_cast<unsigned>(s % 60u));
}

inline const char* work_mode_name(int mode) {
    switch (mode) {
    case 1: return i18n::tr(exc::K::WorkEco);
    case 2: return i18n::tr(exc::K::WorkPower);
    default: return i18n::tr(exc::K::WorkStandard);
    }
}

struct Telemetry {
    int rpm;
    int load;
    int hyd_pressure;
    int coolant_temp;
    int hyd_oil_temp;
    int fuel_tenths;     // 78.0 % -> 780
    int battery_tenths;  // 24.6 V -> 246
    int hours_tenths;    // 1284.5 h -> 12845
    bool engine_on;
    bool hyd_lock;
    bool auto_idle;
    bool maintenance_due;
    bool can_online;
};

inline Telemetry simulate(uint32_t ms) {
    Telemetry t{};
    const int phase = static_cast<int>((ms / 500u) % 24u);  // 12 s cycle

    t.rpm             = 1750 + phase * 12;
    t.load            = 52 + phase;
    t.hyd_pressure    = 230 + (phase % 8) * 4;
    t.coolant_temp    = 84 + phase / 6;
    t.hyd_oil_temp    = 70 + phase / 8;
    t.fuel_tenths     = 780 - static_cast<int>((ms / 30000u) % 200u);
    t.battery_tenths  = 246 - (phase % 6);
    t.hours_tenths    = 12845;
    t.engine_on       = true;
    t.hyd_lock        = (phase % 3) != 0;
    t.auto_idle       = false;
    t.maintenance_due = ((ms / 4000u) % 2u) == 0u;
    t.can_online      = true;
    return t;
}

struct SettingsState {
    bool backlight = true;
    bool buzzer = true;
    bool auto_idle = false;
    bool eco = true;
    int brightness = 80;
    int volume = 60;
    int idle_seconds = 180;
    int language = 0;
    int work_mode = 0;
    bool remember_last = true;
    std::string operator_name;
};

}  // namespace app
