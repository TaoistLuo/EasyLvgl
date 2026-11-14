/**************************************************************************************************
 * @file        main.cpp
 * @brief       工程级示例：挖掘机智能仪表盘（HMI）
 * @details     目标平台为桌面 Linux（SDL2 后端），演示 EasyLVGL 工程化用法：
 *              - 工程清单 + 多屏幕 + 可复用组件（GaugeCard / ParamRow / StatusBadge）+ 图片资源
 *              - luic 生成 UI 类，业务逻辑通过继承生成类实现
 *              - 运行时用 LVGL Tiny TTF 加载完整中文字体（思源黑体同源，16/22 px）
 *              - 多语言走 LVGL 原生 lv_translation：文案源为 Excel，构建时生成静态语言包
 *              - 配置页覆盖 switch / 单选框 / 复选框 / 数值输入 / 下拉框 / 键盘输入
 *
 * @author      taoist.luo
 * @version     1.4.0
 * @date        2026-11-08
 *
 * Copyright (c) 2026 taoist.luo (https://github.com/TaoistLuo/EasyLVGL)
 *
 * MIT License
 *************************************************************************************************/

#include "easylvgl/lvgl_widgets.hpp"
#include "easylvgl/core/signal_handler.hpp"
#include "easylvgl/core/platform.hpp"

#include "Dashboard_ui.hpp"
#include "InfoCenter_ui.hpp"
#include "Settings_ui.hpp"
#include "i18n.hpp"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

using namespace easylvgl;

#ifndef EXCAVATOR_FONT_FILE
#define EXCAVATOR_FONT_FILE "fonts/NotoSansSC-Regular.ttf"
#endif

namespace {

constexpr uint32_t kPageBg = 0x121820;
constexpr uint32_t kGreen = 0x28C76F;
constexpr uint32_t kAmber = 0xFFB300;
constexpr uint32_t kCyan = 0x00D1FF;
constexpr uint32_t kOffBg = 0x2A3542;

std::vector<unsigned char> load_binary_file(const char* path) {
    std::vector<unsigned char> data;
    std::FILE* f = std::fopen(path, "rb");
    if (!f) return data;
    std::fseek(f, 0, SEEK_END);
    const long size = std::ftell(f);
    std::rewind(f);
    if (size > 0) {
        data.resize(static_cast<size_t>(size));
        if (std::fread(data.data(), 1, data.size(), f) != data.size()) data.clear();
    }
    std::fclose(f);
    return data;
}

lv_obj_t* make_page(lv_obj_t* parent, lv_font_t* font) {
    lv_obj_t* page = lv_obj_create(parent);
    lv_obj_remove_style_all(page);
    lv_obj_set_pos(page, 0, 0);
    lv_obj_set_size(page, 800, 480);
    lv_obj_set_style_bg_color(page, lv_color_hex(kPageBg), 0);
    lv_obj_set_style_bg_opa(page, LV_OPA_COVER, 0);
    lv_obj_set_scrollable(page, false);
    if (font) lv_obj_set_style_text_font(page, font, 0);
    lv_obj_set_style_text_color(page, lv_color_hex(0xE8EEF5), 0);
    return page;
}

void set_page_visible(lv_obj_t* page, bool visible) {
    lv_obj_set_hidden(page, !visible);
}

int percent_of(int value, int maximum) {
    if (maximum <= 0) return 0;
    int p = value * 100 / maximum;
    return p < 0 ? 0 : (p > 100 ? 100 : p);
}

void format_uptime(uint32_t ms, char* buf, size_t size) {
    uint32_t s = ms / 1000u;
    std::snprintf(buf, size, "%02u:%02u:%02u",
                  static_cast<unsigned>(s / 3600u % 100u),
                  static_cast<unsigned>(s / 60u % 60u),
                  static_cast<unsigned>(s % 60u));
}

void set_badge(StatusBadge_UI* badge, const char* text, uint32_t color) {
    badge->set_text(text);
    lv_obj_set_style_bg_color(badge->root(), lv_color_hex(color), 0);
}

const char* work_mode_name(int mode) {
    switch (mode) {
    case 1: return i18n::tr(exc::K::WorkEco);
    case 2: return i18n::tr(exc::K::WorkPower);
    default: return i18n::tr(exc::K::WorkStandard);
    }
}

/* ---- 模拟 CAN 采集数据 ---------------------------------------------------- */
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

Telemetry simulate(uint32_t ms) {
    Telemetry t{};
    const int phase = static_cast<int>((ms / 500u) % 24u);  // 12 s cycle

    t.rpm             = 1750 + phase * 12;                  // 1750..2026
    t.load            = 52 + phase;                         // 52..75
    t.hyd_pressure    = 230 + (phase % 8) * 4;              // 230..258
    t.coolant_temp    = 84 + phase / 6;                     // 84..87
    t.hyd_oil_temp    = 70 + phase / 8;                     // 70..72
    t.fuel_tenths     = 780 - static_cast<int>((ms / 30000u) % 200u);
    t.battery_tenths  = 246 - (phase % 6);
    t.hours_tenths    = 12845;
    t.engine_on       = true;
    t.hyd_lock        = (phase % 3) != 0;
    t.auto_idle       = false;
    t.maintenance_due = ((ms / 4000u) % 2u) == 0u;          // 4 s blink
    t.can_online      = true;
    return t;
}

/* ---- 配置状态 -------------------------------------------------------------- */
struct SettingsState {
    bool backlight = true;
    bool buzzer = true;
    bool auto_idle = false;
    bool eco = true;
    int brightness = 80;
    int volume = 60;
    int idle_seconds = 180;
    int language = 0;          // 0=中文, 1=English
    int work_mode = 0;          // 0=标准, 1=节能, 2=重载
    bool remember_last = true;
    std::string operator_name;
};

/* ---- 主仪表盘 -------------------------------------------------------------- */
class DashboardScreen : public Dashboard_UI {
public:
    std::function<void()> on_open_info;
    std::function<void()> on_open_settings;

    void apply_fonts(lv_font_t* body, lv_font_t* title) {
        if (!body) return;
        if (title) lv_obj_set_style_text_font(m_lbl_title->native(), title, 0);
        lv_obj_set_style_text_font(m_btn_info->native(), title ? title : body, 0);
        lv_obj_set_style_text_font(m_btn_settings->native(), title ? title : body, 0);
    }

    void bind_i18n() {
        using exc::K;
        i18n::bind_label(m_lbl_title->native(), K::AppTitle);
        i18n::bind_label(m_lbl_subtitle->native(), K::AppSubtitle);
        i18n::bind_label(m_lbl_status_title->native(), K::StatusTitle);
        i18n::bind_label(m_lbl_footer->native(), K::Footer);
        i18n::bind_text(m_btn_info->native(), K::NavLiveData,
                        [this](const char* text) { m_btn_info->set_text(text); });
        i18n::bind_text(m_btn_settings->native(), K::NavConfig,
                        [this](const char* text) { m_btn_settings->set_text(text); });
        i18n::bind_text(m_gauge_rpm->root(), K::GaugeRpm,
                        [this](const char* text) { m_gauge_rpm->set_caption(text); });
        i18n::bind_text(m_gauge_load->root(), K::GaugeLoad,
                        [this](const char* text) { m_gauge_load->set_caption(text); });
        i18n::bind_text(m_param_fuel->root(), K::FuelLevel,
                        [this](const char* text) { m_param_fuel->set_name(text); });
        i18n::bind_text(m_param_coolant->root(), K::CoolantTemp,
                        [this](const char* text) { m_param_coolant->set_name(text); });
        i18n::bind_text(m_param_batt->root(), K::BatteryVoltage,
                        [this](const char* text) { m_param_batt->set_name(text); });
        i18n::on_language_change(m_lbl_mode->native(), [this] { set_eco(eco); });
    }

    void on_btn_info_clicked(lv_event_t*) override {
        if (on_open_info) on_open_info();
    }

    void on_btn_settings_clicked(lv_event_t*) override {
        if (on_open_settings) on_open_settings();
    }

    void set_eco(bool on) {
        eco = on;
        m_lbl_mode->set_text(i18n::tr(on ? exc::K::ModeEco : exc::K::ModeNormal));
        m_lbl_mode->set_color(on ? kGreen : kCyan);
    }

    void apply(const Telemetry& t, uint32_t ms) {
        char text[32];

        m_gauge_rpm->set_value(t.rpm);
        std::snprintf(text, sizeof text, "%d", t.rpm);
        m_gauge_rpm->set_reading(text);

        m_gauge_load->set_value(t.load);
        std::snprintf(text, sizeof text, "%d", t.load);
        m_gauge_load->set_reading(text);

        std::snprintf(text, sizeof text, "%d", t.fuel_tenths / 10);
        m_param_fuel->set_value(text);
        m_param_fuel->set_level(percent_of(t.fuel_tenths, 1000));

        std::snprintf(text, sizeof text, "%d", t.coolant_temp);
        m_param_coolant->set_value(text);
        m_param_coolant->set_level(percent_of(t.coolant_temp, 120));

        std::snprintf(text, sizeof text, "%d.%d", t.battery_tenths / 10, t.battery_tenths % 10);
        m_param_batt->set_value(text);
        m_param_batt->set_level(percent_of(t.battery_tenths, 300));

        m_lbl_engine->set_text(i18n::tr(t.engine_on ? exc::K::EngineRunning : exc::K::EngineStopped));
        m_lbl_hyd->set_text(i18n::tr(t.hyd_lock ? exc::K::HydReady : exc::K::HydLocked));
        m_lbl_batt->set_text(i18n::tr(t.engine_on ? exc::K::BattCharging : exc::K::BattDischarging));
        m_lbl_maint->set_text(i18n::tr(t.maintenance_due ? exc::K::MaintDue : exc::K::MaintOk));

        if (t.engine_on) m_led_engine->on(); else m_led_engine->off();
        if (t.hyd_lock) m_led_hyd->on(); else m_led_hyd->off();
        if (t.engine_on) m_led_batt->on(); else m_led_batt->off();
        if (t.maintenance_due) m_led_maint->on(); else m_led_maint->off();

        format_uptime(ms, text, sizeof text);
        m_lbl_clock->set_text(text);
    }

private:
    bool eco = true;
};

/* ---- 实时参数查询 ---------------------------------------------------------- */
class InfoCenterScreen : public InfoCenter_UI {
public:
    std::function<void()> on_back;

    void apply_fonts(lv_font_t* body, lv_font_t* title) {
        if (!body) return;
        if (title) lv_obj_set_style_text_font(m_lbl_title->native(), title, 0);
        lv_obj_set_style_text_font(m_btn_back->native(), body, 0);
    }

    void prepare_badges() {
        lv_obj_set_style_bg_opa(m_badge_engine->root(), LV_OPA_COVER, 0);
        lv_obj_set_style_bg_opa(m_badge_hyd->root(), LV_OPA_COVER, 0);
        lv_obj_set_style_bg_opa(m_badge_auto_idle->root(), LV_OPA_COVER, 0);
    }

    void bind_i18n() {
        using exc::K;
        i18n::bind_label(m_lbl_title->native(), K::InfoTitle);
        i18n::bind_label(m_lbl_sub->native(), K::InfoSubtitle);
        i18n::bind_label(m_lbl_matrix_title->native(), K::StatusAlarms);
        i18n::bind_label(m_lbl_engine->native(), K::PromptEngine);
        i18n::bind_label(m_lbl_hyd->native(), K::PromptHydLock);
        i18n::bind_label(m_lbl_auto_idle->native(), K::PromptAutoIdle);
        i18n::bind_label(m_lbl_can->native(), K::PromptCanBus);
        i18n::bind_label(m_lbl_maint->native(), K::PromptMaint);
        i18n::bind_label(m_lbl_oil->native(), K::PromptOilTemp);
        i18n::bind_text(m_btn_back->native(), K::InfoBack,
                        [this](const char* text) { m_btn_back->set_text(text); });
        i18n::bind_text(m_param_rpm->root(), K::ParamEngineSpeed,
                        [this](const char* text) { m_param_rpm->set_name(text); });
        i18n::bind_text(m_param_hyd->root(), K::ParamHydPressure,
                        [this](const char* text) { m_param_hyd->set_name(text); });
        i18n::bind_text(m_param_coolant->root(), K::ParamCoolantTemp,
                        [this](const char* text) { m_param_coolant->set_name(text); });
        i18n::bind_text(m_param_fuel->root(), K::ParamFuelLevel,
                        [this](const char* text) { m_param_fuel->set_name(text); });
        i18n::bind_text(m_param_batt->root(), K::ParamBattery,
                        [this](const char* text) { m_param_batt->set_name(text); });
        i18n::bind_text(m_param_hours->root(), K::ParamWorkHours,
                        [this](const char* text) { m_param_hours->set_name(text); });
        i18n::bind_text(m_param_hyd_oil->root(), K::ParamHydOilTemp,
                        [this](const char* text) { m_param_hyd_oil->set_name(text); });
        i18n::bind_text(m_param_load->root(), K::ParamLoadRate,
                        [this](const char* text) { m_param_load->set_name(text); });
        i18n::on_language_change(m_lbl_ts->native(), [this] { apply(last, last_ms); });
    }

    void on_btn_back_clicked(lv_event_t*) override {
        if (on_back) on_back();
    }

    void apply(const Telemetry& t, uint32_t ms) {
        last = t;
        last_ms = ms;
        char text[32];
        char clock[16];

        std::snprintf(text, sizeof text, "%d", t.rpm);
        m_param_rpm->set_value(text);
        m_param_rpm->set_level(percent_of(t.rpm, 3000));

        std::snprintf(text, sizeof text, "%d", t.hyd_pressure);
        m_param_hyd->set_value(text);
        m_param_hyd->set_level(percent_of(t.hyd_pressure, 400));

        std::snprintf(text, sizeof text, "%d", t.coolant_temp);
        m_param_coolant->set_value(text);
        m_param_coolant->set_level(percent_of(t.coolant_temp, 120));

        std::snprintf(text, sizeof text, "%d", t.fuel_tenths / 10);
        m_param_fuel->set_value(text);
        m_param_fuel->set_level(percent_of(t.fuel_tenths, 1000));

        std::snprintf(text, sizeof text, "%d.%d", t.battery_tenths / 10, t.battery_tenths % 10);
        m_param_batt->set_value(text);
        m_param_batt->set_level(percent_of(t.battery_tenths, 300));

        std::snprintf(text, sizeof text, "%d.%d", t.hours_tenths / 10, t.hours_tenths % 10);
        m_param_hours->set_value(text);
        m_param_hours->set_level(percent_of(t.hours_tenths % 1000, 1000));

        std::snprintf(text, sizeof text, "%d", t.hyd_oil_temp);
        m_param_hyd_oil->set_value(text);
        m_param_hyd_oil->set_level(percent_of(t.hyd_oil_temp, 120));

        std::snprintf(text, sizeof text, "%d", t.load);
        m_param_load->set_value(text);
        m_param_load->set_level(percent_of(t.load, 100));

        set_badge(m_badge_engine, i18n::tr(t.engine_on ? exc::K::StateOn : exc::K::StateOff),
                  t.engine_on ? kGreen : kOffBg);
        set_badge(m_badge_hyd, i18n::tr(t.hyd_lock ? exc::K::StateOn : exc::K::StateOff),
                  t.hyd_lock ? kGreen : kOffBg);
        set_badge(m_badge_auto_idle, i18n::tr(t.auto_idle ? exc::K::StateOn : exc::K::StateOff),
                  t.auto_idle ? kGreen : kOffBg);

        if (t.can_online) m_led_can->on(); else m_led_can->off();
        m_lbl_can_state->set_text(i18n::tr(t.can_online ? exc::K::CanOnline : exc::K::CanOffline));

        if (t.maintenance_due) m_led_maint->on(); else m_led_maint->off();
        m_lbl_maint_state->set_text(i18n::tr(t.maintenance_due ? exc::K::MaintDueShort : exc::K::MaintOkShort));

        std::snprintf(text, sizeof text, "%d °C", t.hyd_oil_temp);
        m_lbl_oil_value->set_text(text);

        format_uptime(ms, clock, sizeof clock);
        std::snprintf(text, sizeof text, "T+%s", clock);
        m_lbl_ts->set_text(text);
    }

private:
    Telemetry last{};
    uint32_t last_ms = 0;
};

/* ---- 配置设置 -------------------------------------------------------------- */
class SettingsScreen : public Settings_UI {
public:
    std::function<void()> on_back;
    std::function<void(const SettingsState&)> on_state_changed;
    std::function<void(i18n::Lang)> on_language_changed;

    void apply_fonts(lv_font_t* body, lv_font_t* title) {
        if (!body) return;
        if (title) lv_obj_set_style_text_font(m_lbl_title->native(), title, 0);
        lv_obj_set_style_text_font(m_btn_back->native(), body, 0);
        lv_obj_set_style_text_font(m_btn_save->native(), body, 0);
        lv_obj_set_style_text_font(m_btn_reset->native(), body, 0);
        lv_obj_set_style_text_font(m_btn_back_bottom->native(), body, 0);
        lv_obj_set_style_text_font(m_radio_std->native(), body, 0);
        lv_obj_set_style_text_font(m_radio_eco->native(), body, 0);
        lv_obj_set_style_text_font(m_radio_power->native(), body, 0);
        lv_obj_set_style_text_font(m_cb_remember->native(), body, 0);
        lv_obj_set_style_text_font(m_edit_operator->native(), body, 0);
        lv_obj_set_scrollbar_mode(m_edit_operator->native(), LV_SCROLLBAR_MODE_OFF);
        lv_dropdown_set_symbol(m_dd_lang->native(), nullptr);
        if (lv_obj_t* list = lv_dropdown_get_list(m_dd_lang->native())) {
            lv_obj_set_style_text_font(list, body, 0);
            lv_obj_set_style_bg_color(list, lv_color_hex(0x1C2530), 0);
            lv_obj_set_style_bg_opa(list, LV_OPA_COVER, 0);
            lv_obj_set_style_text_color(list, lv_color_hex(0xE8EEF5), 0);
        }

        auto light_text = [](lv_obj_t* obj) {
            lv_obj_set_style_text_color(obj, lv_color_hex(0xE8EEF5), 0);
        };
        light_text(m_radio_std->native());
        light_text(m_radio_eco->native());
        light_text(m_radio_power->native());
        light_text(m_cb_remember->native());
        light_text(m_dd_lang->native());

        auto style_radio = [](LRadioButton* radio) {
            lv_obj_t* ring = lv_obj_get_child(radio->native(), 0);
            if (!ring) return;
            lv_obj_set_style_border_color(ring, lv_color_hex(0xB8C4D0), 0);
            if (lv_obj_t* dot = lv_obj_get_child(ring, 0)) {
                lv_obj_set_style_bg_color(dot, lv_color_hex(kCyan), 0);
            }
            if (lv_obj_t* label = lv_obj_get_child(radio->native(), 1)) {
                lv_obj_set_style_text_color(label, lv_color_hex(0xE8EEF5), 0);
            }
        };
        style_radio(m_radio_std);
        style_radio(m_radio_eco);
        style_radio(m_radio_power);

        auto hide_cursor = [](lv_obj_t* obj) {
            lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_PART_CURSOR);
            lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_PART_SELECTED);
        };
        hide_cursor(m_spin_bright->native());
        hide_cursor(m_spin_volume->native());
        hide_cursor(m_spin_idle->native());
    }

    void bind_i18n() {
        using exc::K;
        i18n::bind_label(m_lbl_title->native(), K::SettingsTitle);
        i18n::bind_label(m_lbl_sub->native(), K::SettingsSubtitle);
        i18n::bind_label(m_lbl_system->native(), K::SectionSystem);
        i18n::bind_label(m_lbl_work_mode->native(), K::SectionWorkMode);
        i18n::bind_label(m_lbl_params->native(), K::SectionParams);
        i18n::bind_label(m_lbl_sw_backlight->native(), K::SwBacklight);
        i18n::bind_label(m_lbl_sw_buzzer->native(), K::SwBuzzer);
        i18n::bind_label(m_lbl_sw_auto_idle->native(), K::SwAutoIdle);
        i18n::bind_label(m_lbl_sw_eco->native(), K::SwEco);
        i18n::bind_label(m_lbl_bright->native(), K::LabelBrightness);
        i18n::bind_label(m_lbl_volume->native(), K::LabelVolume);
        i18n::bind_label(m_lbl_idle->native(), K::LabelIdle);
        i18n::bind_label(m_lbl_lang->native(), K::LabelLanguage);
        i18n::bind_label(m_lbl_operator->native(), K::LabelOperator);
        i18n::bind_label(m_lbl_hint->native(), K::SettingsHint);
        i18n::bind_label(m_lbl_footer->native(), K::SettingsFooter);
        i18n::bind_text(m_btn_back->native(), K::SettingsBack,
                        [this](const char* text) { m_btn_back->set_text(text); });
        i18n::bind_text(m_btn_save->native(), K::BtnSave,
                        [this](const char* text) { m_btn_save->set_text(text); });
        i18n::bind_text(m_btn_reset->native(), K::BtnReset,
                        [this](const char* text) { m_btn_reset->set_text(text); });
        i18n::bind_text(m_btn_back_bottom->native(), K::BtnBack,
                        [this](const char* text) { m_btn_back_bottom->set_text(text); });
        i18n::bind_text(m_radio_std->native(), K::WorkStandard,
                        [this](const char* text) { m_radio_std->set_text(text); });
        i18n::bind_text(m_radio_eco->native(), K::WorkEco,
                        [this](const char* text) { m_radio_eco->set_text(text); });
        i18n::bind_text(m_radio_power->native(), K::WorkPower,
                        [this](const char* text) { m_radio_power->set_text(text); });
        i18n::bind_text(m_cb_remember->native(), K::LabelRemember,
                        [this](const char* text) { m_cb_remember->set_text(text); });
        i18n::bind_text(m_edit_operator->native(), K::OperatorPlaceholder,
                        [this](const char* text) { m_edit_operator->set_placeholder(text); });
        i18n::on_language_change(m_lbl_feedback->native(), [this] { render_feedback(); });
        m_dd_lang->set_selected(static_cast<uint16_t>(i18n::current()));
        render_feedback();
    }

    void on_btn_back_clicked(lv_event_t*) override { go_back(); }
    void on_btn_back_bottom_clicked(lv_event_t*) override { go_back(); }

    void on_sw_backlight_value_changed(lv_event_t*) override { notify_pending(); }
    void on_sw_buzzer_value_changed(lv_event_t*) override { notify_pending(); }
    void on_sw_auto_idle_value_changed(lv_event_t*) override { notify_pending(); }
    void on_sw_eco_value_changed(lv_event_t*) override { notify_pending(); }

    void on_radio_std_value_changed(lv_event_t*) override { select_work_mode(0); }
    void on_radio_eco_value_changed(lv_event_t*) override { select_work_mode(1); }
    void on_radio_power_value_changed(lv_event_t*) override { select_work_mode(2); }

    void on_spin_bright_value_changed(lv_event_t*) override { notify_pending(); }
    void on_spin_volume_value_changed(lv_event_t*) override { notify_pending(); }
    void on_spin_idle_value_changed(lv_event_t*) override { notify_pending(); }
    void on_cb_remember_value_changed(lv_event_t*) override { notify_pending(); }

    void on_dd_lang_value_changed(lv_event_t*) override {
        const uint16_t index = m_dd_lang->selected();
        const i18n::Lang selected = index == 1 ? i18n::Lang::En
                                   : (index == 2 ? i18n::Lang::Ru : i18n::Lang::Zh);
        if (selected == i18n::current()) return;
        i18n::set_language(selected);
        if (on_language_changed) on_language_changed(selected);
    }

    void on_edit_operator_clicked(lv_event_t*) override {
        lv_obj_set_hidden(m_kb_operator->native(), false);
    }

    void on_edit_operator_ready(lv_event_t*) override {
        lv_obj_set_hidden(m_kb_operator->native(), true);
        notify_pending();
    }

    void on_btn_save_clicked(lv_event_t*) override {
        SettingsState s = current();
        std::printf("[SETTINGS] saved: backlight=%d buzzer=%d auto_idle=%d eco=%d "
                    "bright=%d volume=%d idle=%ds lang=%d mode=%s remember=%d operator='%s'\n",
                    s.backlight, s.buzzer, s.auto_idle, s.eco,
                    s.brightness, s.volume, s.idle_seconds, s.language,
                    work_mode_name(s.work_mode), s.remember_last, s.operator_name.c_str());
        feedback = Feedback::Saved;
        render_feedback();
        if (on_state_changed) on_state_changed(s);
    }

    void on_btn_reset_clicked(lv_event_t*) override {
        restore_defaults();
        feedback = Feedback::Reset;
        render_feedback();
        std::printf("[SETTINGS] restored defaults\n");
    }

    SettingsState current() const {
        SettingsState s;
        s.backlight = m_sw_backlight->is_checked();
        s.buzzer = m_sw_buzzer->is_checked();
        s.auto_idle = m_sw_auto_idle->is_checked();
        s.eco = m_sw_eco->is_checked();
        s.brightness = static_cast<int>(m_spin_bright->value());
        s.volume = static_cast<int>(m_spin_volume->value());
        s.idle_seconds = static_cast<int>(m_spin_idle->value());
        s.language = static_cast<int>(m_dd_lang->selected());
        s.work_mode = m_radio_eco->is_checked() ? 1 : (m_radio_power->is_checked() ? 2 : 0);
        s.remember_last = m_cb_remember->is_checked();
        const char* name = m_edit_operator->text();
        s.operator_name = name ? name : "";
        return s;
    }

private:
    enum class Feedback { Idle, Pending, Saved, Reset };
    Feedback feedback = Feedback::Idle;

    void go_back() {
        lv_obj_set_hidden(m_kb_operator->native(), true);
        if (on_back) on_back();
    }

    void select_work_mode(int mode) {
        m_radio_std->set_checked(mode == 0);
        m_radio_eco->set_checked(mode == 1);
        m_radio_power->set_checked(mode == 2);
        notify_pending();
    }

    void restore_defaults() {
        lv_obj_set_hidden(m_kb_operator->native(), true);
        m_sw_backlight->set_checked(true);
        m_sw_buzzer->set_checked(true);
        m_sw_auto_idle->set_checked(false);
        m_sw_eco->set_checked(true);
        m_radio_std->set_checked(true);
        m_radio_eco->set_checked(false);
        m_radio_power->set_checked(false);
        m_spin_bright->set_value(80);
        m_spin_volume->set_value(60);
        m_spin_idle->set_value(180);
        m_dd_lang->set_selected(static_cast<uint16_t>(i18n::current()));
        m_cb_remember->set_checked(true);
        m_edit_operator->set_text("");
        if (on_state_changed) on_state_changed(current());
    }

    void notify_pending() {
        feedback = Feedback::Pending;
        render_feedback();
        if (on_state_changed) on_state_changed(current());
    }

    void render_feedback() {
        char text[128];
        uint32_t color = kGreen;
        const SettingsState s = current();
        switch (feedback) {
        case Feedback::Pending:
            std::snprintf(text, sizeof text, i18n::tr(exc::K::FbPendingFmt),
                          work_mode_name(s.work_mode), s.brightness, s.volume);
            color = kAmber;
            break;
        case Feedback::Saved:
            std::snprintf(text, sizeof text, i18n::tr(exc::K::FbSavedFmt),
                          work_mode_name(s.work_mode), s.brightness, s.volume);
            break;
        case Feedback::Reset:
            std::snprintf(text, sizeof text, "%s", i18n::tr(exc::K::FbReset));
            color = kAmber;
            break;
        default:
            std::snprintf(text, sizeof text, "%s", i18n::tr(exc::K::FbIdle));
            break;
        }
        m_lbl_feedback->set_color(color).set_text(text);
    }
};

const char* lang_code(i18n::Lang lang) {
    switch (lang) {
    case i18n::Lang::En: return "en";
    case i18n::Lang::Ru: return "ru";
    default: return "zh";
    }
}

const char* lang_name(i18n::Lang lang) {
    switch (lang) {
    case i18n::Lang::En: return "English";
    case i18n::Lang::Ru: return "Русский";
    default: return "中文";
    }
}

i18n::Lang lang_from_env() {
    const char* value = std::getenv("EASYLVGL_LANG");
    if (value && (std::strcmp(value, "ru") == 0 || std::strcmp(value, "RU") == 0)) {
        return i18n::Lang::Ru;
    }
    if (value && (std::strcmp(value, "en") == 0 || std::strcmp(value, "EN") == 0)) {
        return i18n::Lang::En;
    }
    return i18n::Lang::Zh;
}

}  // namespace

int main() {
    lv_init();
    if (!init_desktop_display(800, 480)) {
        std::fprintf(stderr, "failed to create desktop display\n");
        return 1;
    }
    if (lv_display_t* disp = lv_display_get_default()) {
        lv_sysmon_hide_performance(disp);
    }

    /* 原生翻译：先注册 Excel 生成的语言包，再选择初始语言 */
    i18n::init(lang_from_env());

    /* 中文：运行时 Tiny TTF，加载完整思源黑体并创建 16 / 22 px 两个字号 */
    std::vector<unsigned char> font_data = load_binary_file(EXCAVATOR_FONT_FILE);
    lv_font_t* font16 = nullptr;
    lv_font_t* font22 = nullptr;
#if LV_USE_TINY_TTF
    if (!font_data.empty()) {
        font16 = lv_tiny_ttf_create_data(font_data.data(), font_data.size(), 16);
        font22 = lv_tiny_ttf_create_data(font_data.data(), font_data.size(), 22);
    }
#endif
    if (!font16 || !font22) {
        std::fprintf(stderr, "warning: font not loaded (%s), falling back to built-in fonts\n",
                     EXCAVATOR_FONT_FILE);
    }

    SignalHandler signal_handler;

    lv_obj_t* page_dashboard = make_page(lv_scr_act(), font16);
    lv_obj_t* page_info = make_page(lv_scr_act(), font16);
    lv_obj_t* page_settings = make_page(lv_scr_act(), font16);

    DashboardScreen dashboard;
    InfoCenterScreen info;
    SettingsScreen settings;

    dashboard.setup(page_dashboard);
    dashboard.bind_events();

    info.setup(page_info);
    info.bind_events();

    settings.setup(page_settings);
    settings.bind_events();

    dashboard.apply_fonts(font16, font22);
    info.apply_fonts(font16, font22);
    info.prepare_badges();
    settings.apply_fonts(font16, font22);

    dashboard.bind_i18n();
    info.bind_i18n();
    settings.bind_i18n();

    dashboard.set_eco(settings.current().eco);

    const Telemetry initial = simulate(0);
    dashboard.apply(initial, 0);
    info.apply(initial, 0);

    set_page_visible(page_info, false);
    set_page_visible(page_settings, false);

    int current_page = 0;
    auto show_page = [&](int index) {
        set_page_visible(page_dashboard, index == 0);
        set_page_visible(page_info, index == 1);
        set_page_visible(page_settings, index == 2);
        current_page = index;
    };

    dashboard.on_open_info = [&] {
        std::printf("[NAV] live data\n");
        show_page(1);
    };
    dashboard.on_open_settings = [&] {
        std::printf("[NAV] configuration\n");
        show_page(2);
    };
    info.on_back = [&] {
        std::printf("[NAV] dashboard\n");
        show_page(0);
    };
    settings.on_back = [&] {
        std::printf("[NAV] dashboard\n");
        show_page(0);
    };
    settings.on_state_changed = [&](const SettingsState& s) {
        dashboard.set_eco(s.eco);
    };
    settings.on_language_changed = [&](i18n::Lang lang) {
        std::printf("[LANG] %s\n", lang_code(lang));
    };

    const uint32_t start_ms = static_cast<uint32_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    const int auto_close_ms = env_int("EASYLVGL_AUTOCLOSE_MS", 0);
    uint32_t last_sample = 0;

    std::printf("============================================\n");
    std::printf("  EXCAVATOR HMI  (project-level example, Linux)\n");
    std::printf("  screens: Dashboard / InfoCenter / Settings\n");
    std::printf("  language: %s (LVGL lv_translation)\n", lang_name(i18n::current()));
    std::printf("  font: %s\n", EXCAVATOR_FONT_FILE);
    std::printf("  Press Ctrl+C to exit.\n");
    std::printf("============================================\n");

    while (signal_handler.is_running()) {
        const uint32_t now_ms = static_cast<uint32_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count()) - start_ms;

        if (now_ms - last_sample >= 500u) {
            last_sample = now_ms;
            Telemetry t = simulate(now_ms);
            dashboard.apply(t, now_ms);
            info.apply(t, now_ms);
        }

        if (auto_close_ms > 0 && now_ms >= static_cast<uint32_t>(auto_close_ms)) {
            std::printf("[DEMO] auto close after %d ms (page %d)\n", auto_close_ms, current_page);
            break;
        }

        lv_timer_handler();
        sleep_ms(5);
    }

#if LV_USE_TINY_TTF
    if (font22) lv_tiny_ttf_destroy(font22);
    if (font16) lv_tiny_ttf_destroy(font16);
#endif

    std::printf("EXCAVATOR HMI stopped.\n");
    return 0;
}
