#pragma once

#include "app_common.hpp"
#include "Dashboard_ui.hpp"

#include <functional>

class Dashboard : public easylvgl::Dashboard_UI {
public:
    explicit Dashboard(lv_obj_t* parent = nullptr);
    ~Dashboard() override = default;

    void apply_fonts(lv_font_t* body, lv_font_t* title);
    void bind_i18n();
    void set_eco(bool on);
    void apply(const app::Telemetry& t, uint32_t ms);

    std::function<void()> on_open_info;
    std::function<void()> on_open_settings;

protected:
    void on_btn_info_clicked(lv_event_t* e) override;
    void on_btn_settings_clicked(lv_event_t* e) override;

private:
    bool eco = true;
};
