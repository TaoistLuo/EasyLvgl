#include "Dashboard.h"

using namespace app;

Dashboard::Dashboard(lv_obj_t* parent) {
    setup(parent);
    bind_events();
}

void Dashboard::apply_fonts(lv_font_t* body, lv_font_t* title) {
    if (!body) return;
    if (title) lv_obj_set_style_text_font(m_lbl_title->native(), title, 0);
    lv_obj_set_style_text_font(m_btn_info->native(), title ? title : body, 0);
    lv_obj_set_style_text_font(m_btn_settings->native(), title ? title : body, 0);
}

void Dashboard::bind_i18n() {
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

void Dashboard::on_btn_info_clicked(lv_event_t*) {
    if (on_open_info) on_open_info();
}

void Dashboard::on_btn_settings_clicked(lv_event_t*) {
    if (on_open_settings) on_open_settings();
}

void Dashboard::set_eco(bool on) {
    eco = on;
    m_lbl_mode->set_text(i18n::tr(on ? exc::K::ModeEco : exc::K::ModeNormal));
    m_lbl_mode->set_color(on ? kGreen : kCyan);
}

void Dashboard::apply(const Telemetry& t, uint32_t ms) {
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
