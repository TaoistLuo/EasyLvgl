#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LCalendar : public LWidget {
public:
    explicit LCalendar(lv_obj_t* parent = nullptr)
        : LWidget(lv_calendar_create(parent ? parent : lv_scr_act())) {}

    LCalendar& today_date(uint32_t year, uint32_t month, uint32_t day) {
        lv_calendar_set_today_date(m_obj, year, month, day);
        return *this;
    }

    LCalendar& showed_date(uint32_t year, uint32_t month) {
        lv_calendar_set_showed_date(m_obj, year, month);
        return *this;
    }

    const lv_calendar_date_t* get_today_date() const {
        return lv_calendar_get_today_date(m_obj);
    }

    const lv_calendar_date_t* get_showed_date() const {
        return lv_calendar_get_showed_date(m_obj);
    }
};

} // namespace lvgl
