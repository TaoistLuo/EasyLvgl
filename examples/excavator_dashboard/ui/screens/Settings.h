#pragma once

#include <functional>

#include "Settings_ui.hpp"
#include "app_common.hpp"

class Settings : public easylvgl::Settings_UI {
 public:
  explicit Settings(lv_obj_t* parent = nullptr);
  ~Settings() override = default;

  void apply_fonts(lv_font_t* body, lv_font_t* title);
  void bind_i18n();
  app::SettingsState current() const;

  std::function<void()> on_back;
  std::function<void(const app::SettingsState&)> on_state_changed;
  std::function<void(i18n::Lang)> on_language_changed;

 protected:
  void on_btn_back_clicked(lv_event_t* e) override;
  void on_btn_back_bottom_clicked(lv_event_t* e) override;
  void on_sw_backlight_value_changed(lv_event_t* e) override;
  void on_sw_buzzer_value_changed(lv_event_t* e) override;
  void on_sw_auto_idle_value_changed(lv_event_t* e) override;
  void on_sw_eco_value_changed(lv_event_t* e) override;
  void on_radio_std_value_changed(lv_event_t* e) override;
  void on_radio_eco_value_changed(lv_event_t* e) override;
  void on_radio_power_value_changed(lv_event_t* e) override;
  void on_spin_bright_value_changed(lv_event_t* e) override;
  void on_spin_volume_value_changed(lv_event_t* e) override;
  void on_spin_idle_value_changed(lv_event_t* e) override;
  void on_cb_remember_value_changed(lv_event_t* e) override;
  void on_dd_lang_value_changed(lv_event_t* e) override;
  void on_edit_operator_clicked(lv_event_t* e) override;
  void on_edit_operator_ready(lv_event_t* e) override;
  void on_btn_save_clicked(lv_event_t* e) override;
  void on_btn_reset_clicked(lv_event_t* e) override;

 private:
  enum class Feedback { Idle, Pending, Saved, Reset };
  Feedback feedback = Feedback::Idle;

  void go_back();
  void select_work_mode(int mode);
  void restore_defaults();
  void notify_pending();
  void render_feedback();
};
