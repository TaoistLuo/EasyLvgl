#include "Settings.h"

using namespace app;

Settings::Settings(lv_obj_t* parent) {
  setup(parent);
  bind_events();
}

void Settings::apply_fonts(lv_font_t* body, lv_font_t* title) {
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

void Settings::bind_i18n() {
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
  i18n::bind_text(
      m_btn_back_bottom->native(), K::BtnBack,
      [this](const char* text) { m_btn_back_bottom->set_text(text); });
  i18n::bind_text(m_radio_std->native(), K::WorkStandard,
                  [this](const char* text) { m_radio_std->set_text(text); });
  i18n::bind_text(m_radio_eco->native(), K::WorkEco,
                  [this](const char* text) { m_radio_eco->set_text(text); });
  i18n::bind_text(m_radio_power->native(), K::WorkPower,
                  [this](const char* text) { m_radio_power->set_text(text); });
  i18n::bind_text(m_cb_remember->native(), K::LabelRemember,
                  [this](const char* text) { m_cb_remember->set_text(text); });
  i18n::bind_text(
      m_edit_operator->native(), K::OperatorPlaceholder,
      [this](const char* text) { m_edit_operator->set_placeholder(text); });
  i18n::on_language_change(m_lbl_feedback->native(),
                           [this] { render_feedback(); });
  m_dd_lang->set_selected(static_cast<uint16_t>(i18n::current()));
  render_feedback();
}

void Settings::on_btn_back_clicked(lv_event_t*) { go_back(); }
void Settings::on_btn_back_bottom_clicked(lv_event_t*) { go_back(); }

void Settings::on_sw_backlight_value_changed(lv_event_t*) { notify_pending(); }
void Settings::on_sw_buzzer_value_changed(lv_event_t*) { notify_pending(); }
void Settings::on_sw_auto_idle_value_changed(lv_event_t*) { notify_pending(); }
void Settings::on_sw_eco_value_changed(lv_event_t*) { notify_pending(); }

void Settings::on_radio_std_value_changed(lv_event_t*) { select_work_mode(0); }
void Settings::on_radio_eco_value_changed(lv_event_t*) { select_work_mode(1); }
void Settings::on_radio_power_value_changed(lv_event_t*) {
  select_work_mode(2);
}

void Settings::on_spin_bright_value_changed(lv_event_t*) { notify_pending(); }
void Settings::on_spin_volume_value_changed(lv_event_t*) { notify_pending(); }
void Settings::on_spin_idle_value_changed(lv_event_t*) { notify_pending(); }
void Settings::on_cb_remember_value_changed(lv_event_t*) { notify_pending(); }

void Settings::on_dd_lang_value_changed(lv_event_t*) {
  const uint16_t index = m_dd_lang->selected();
  const i18n::Lang selected =
      index == 1 ? i18n::Lang::En
                 : (index == 2 ? i18n::Lang::Ru : i18n::Lang::Zh);
  if (selected == i18n::current()) return;
  i18n::set_language(selected);
  if (on_language_changed) on_language_changed(selected);
}

void Settings::on_edit_operator_clicked(lv_event_t*) {
  lv_obj_set_hidden(m_kb_operator->native(), false);
}

void Settings::on_edit_operator_ready(lv_event_t*) {
  lv_obj_set_hidden(m_kb_operator->native(), true);
  notify_pending();
}

void Settings::on_btn_save_clicked(lv_event_t*) {
  SettingsState s = current();
  std::printf(
      "[SETTINGS] saved: backlight=%d buzzer=%d auto_idle=%d eco=%d "
      "bright=%d volume=%d idle=%ds lang=%d mode=%s remember=%d "
      "operator='%s'\n",
      s.backlight, s.buzzer, s.auto_idle, s.eco, s.brightness, s.volume,
      s.idle_seconds, s.language, work_mode_name(s.work_mode), s.remember_last,
      s.operator_name.c_str());
  feedback = Feedback::Saved;
  render_feedback();
  if (on_state_changed) on_state_changed(s);
}

void Settings::on_btn_reset_clicked(lv_event_t*) {
  restore_defaults();
  feedback = Feedback::Reset;
  render_feedback();
  std::printf("[SETTINGS] restored defaults\n");
}

SettingsState Settings::current() const {
  SettingsState s;
  s.backlight = m_sw_backlight->is_checked();
  s.buzzer = m_sw_buzzer->is_checked();
  s.auto_idle = m_sw_auto_idle->is_checked();
  s.eco = m_sw_eco->is_checked();
  s.brightness = static_cast<int>(m_spin_bright->value());
  s.volume = static_cast<int>(m_spin_volume->value());
  s.idle_seconds = static_cast<int>(m_spin_idle->value());
  s.language = static_cast<int>(m_dd_lang->selected());
  s.work_mode =
      m_radio_eco->is_checked() ? 1 : (m_radio_power->is_checked() ? 2 : 0);
  s.remember_last = m_cb_remember->is_checked();
  const char* name = m_edit_operator->text();
  s.operator_name = name ? name : "";
  return s;
}

void Settings::go_back() {
  lv_obj_set_hidden(m_kb_operator->native(), true);
  if (on_back) on_back();
}

void Settings::select_work_mode(int mode) {
  m_radio_std->set_checked(mode == 0);
  m_radio_eco->set_checked(mode == 1);
  m_radio_power->set_checked(mode == 2);
  notify_pending();
}

void Settings::restore_defaults() {
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

void Settings::notify_pending() {
  feedback = Feedback::Pending;
  render_feedback();
  if (on_state_changed) on_state_changed(current());
}

void Settings::render_feedback() {
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
