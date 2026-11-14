#pragma once

#include "app_common.hpp"
#include "InfoCenter_ui.hpp"

#include <functional>

class InfoCenter : public easylvgl::InfoCenter_UI {
public:
    explicit InfoCenter(lv_obj_t* parent = nullptr);
    ~InfoCenter() override = default;

    void apply_fonts(lv_font_t* body, lv_font_t* title);
    void prepare_badges();
    void bind_i18n();
    void apply(const app::Telemetry& t, uint32_t ms);

    std::function<void()> on_back;

protected:
    void on_btn_back_clicked(lv_event_t* e) override;

private:
    app::Telemetry last{};
    uint32_t last_ms = 0;
};
