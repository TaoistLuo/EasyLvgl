#include "InfoCenter.h"

using namespace app;

namespace {
void set_badge(StatusBadge_UI* badge, const char* text, uint32_t color) {
  badge->set_text(text);
  lv_obj_set_style_bg_color(badge->root(), lv_color_hex(color), 0);
}
}  // namespace

InfoCenter::InfoCenter(lv_obj_t* parent) {
  setup(parent);
  bind_events();
}

void InfoCenter::apply_fonts(lv_font_t* body, lv_font_t* title) {
  if (!body) return;
  if (title) lv_obj_set_style_text_font(m_lbl_title->native(), title, 0);
  lv_obj_set_style_text_font(m_btn_back->native(), body, 0);
}

void InfoCenter::prepare_badges() {
  lv_obj_set_style_bg_opa(m_badge_engine->root(), LV_OPA_COVER, 0);
  lv_obj_set_style_bg_opa(m_badge_hyd->root(), LV_OPA_COVER, 0);
  lv_obj_set_style_bg_opa(m_badge_auto_idle->root(), LV_OPA_COVER, 0);
}

void InfoCenter::bind_i18n() {
  using exc::K;
  i18n::bind_label(m_lbl_title->native(), K::InfoTitle);
  i18n::bind_label(m_lbl_sub->native(), K::InfoSubtitle);
  i18n::bind_label(m_lbl_matrix_title->native(), K::StatusAlarms);
  i18n::bind_label(m_lbl_engine->native(), K::PromptEngine);
  i18n::bind_label(m_lbl_hyd->native(), K::PromptHydLock);
  i18n::bind_label(m_lbl_auto_idle->native(), K::PromptAutoIdle);
  i18n::bind_label(m_lbl_can->native(), K::PromptCanBus);
  i18n::bind_label(m_lbl_maint->native(), K::PromptMaint);
  i18n::bind_text(m_btn_back->native(), K::InfoBack,
                  [this](const char* text) { m_btn_back->set_text(text); });
  i18n::bind_text(m_param_rpm->root(), K::ParamEngineSpeed,
                  [this](const char* text) { m_param_rpm->set_name(text); });
  i18n::bind_text(m_param_hyd->root(), K::ParamHydPressure,
                  [this](const char* text) { m_param_hyd->set_name(text); });
  i18n::bind_text(
      m_param_coolant->root(), K::ParamCoolantTemp,
      [this](const char* text) { m_param_coolant->set_name(text); });
  i18n::bind_text(m_param_fuel->root(), K::ParamFuelLevel,
                  [this](const char* text) { m_param_fuel->set_name(text); });
  i18n::bind_text(m_param_batt->root(), K::ParamBattery,
                  [this](const char* text) { m_param_batt->set_name(text); });
  i18n::bind_text(m_param_hours->root(), K::ParamWorkHours,
                  [this](const char* text) { m_param_hours->set_name(text); });
  i18n::bind_text(
      m_param_hyd_oil->root(), K::ParamHydOilTemp,
      [this](const char* text) { m_param_hyd_oil->set_name(text); });
  i18n::bind_text(m_param_load->root(), K::ParamLoadRate,
                  [this](const char* text) { m_param_load->set_name(text); });
  i18n::on_language_change(m_lbl_ts->native(),
                           [this] { apply(last, last_ms); });
}

void InfoCenter::on_btn_back_clicked(lv_event_t*) {
  if (on_back) on_back();
}

void InfoCenter::apply(const Telemetry& t, uint32_t ms) {
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

  std::snprintf(text, sizeof text, "%d.%d", t.battery_tenths / 10,
                t.battery_tenths % 10);
  m_param_batt->set_value(text);
  m_param_batt->set_level(percent_of(t.battery_tenths, 300));

  std::snprintf(text, sizeof text, "%d.%d", t.hours_tenths / 10,
                t.hours_tenths % 10);
  m_param_hours->set_value(text);
  m_param_hours->set_level(percent_of(t.hours_tenths % 1000, 1000));

  std::snprintf(text, sizeof text, "%d", t.hyd_oil_temp);
  m_param_hyd_oil->set_value(text);
  m_param_hyd_oil->set_level(percent_of(t.hyd_oil_temp, 120));

  std::snprintf(text, sizeof text, "%d", t.load);
  m_param_load->set_value(text);
  m_param_load->set_level(percent_of(t.load, 100));

  set_badge(m_badge_engine,
            i18n::tr(t.engine_on ? exc::K::StateOn : exc::K::StateOff),
            t.engine_on ? kGreen : kOffBg);
  set_badge(m_badge_hyd,
            i18n::tr(t.hyd_lock ? exc::K::StateOn : exc::K::StateOff),
            t.hyd_lock ? kGreen : kOffBg);
  set_badge(m_badge_auto_idle,
            i18n::tr(t.auto_idle ? exc::K::StateOn : exc::K::StateOff),
            t.auto_idle ? kGreen : kOffBg);

  if (t.can_online)
    m_led_can->on();
  else
    m_led_can->off();
  m_lbl_can_state->set_text(
      i18n::tr(t.can_online ? exc::K::CanOnline : exc::K::CanOffline));

  if (t.maintenance_due)
    m_led_maint->on();
  else
    m_led_maint->off();
  m_lbl_maint_state->set_text(i18n::tr(
      t.maintenance_due ? exc::K::MaintDueShort : exc::K::MaintOkShort));

  std::snprintf(text, sizeof text, "%d °C", t.hyd_oil_temp);
  m_oil_row->set_label(i18n::tr(exc::K::PromptOilTemp));
  m_oil_row->set_value(text);

  format_uptime(ms, clock, sizeof clock);
  std::snprintf(text, sizeof text, "T+%s", clock);
  m_lbl_ts->set_text(text);
}
