#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LCalendar : public LWidgetCRTP<LCalendar> {
public:
    explicit LCalendar(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_calendar_create(parent ? parent : lv_scr_act())) {}

    LCalendar& set_today_date(uint32_t year, uint32_t month, uint32_t day) {
        lv_calendar_set_today_date(m_obj, year, month, day);
        return *this;
    }

    LCalendar& set_showed_date(uint32_t year, uint32_t month) {
        lv_calendar_set_showed_date(m_obj, year, month);
        return *this;
    }

    const lv_calendar_date_t* today_date() const {
        return lv_calendar_get_today_date(m_obj);
    }

    const lv_calendar_date_t* showed_date() const {
        return lv_calendar_get_showed_date(m_obj);
    }

    LCalendar& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LCalendar& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
};

} // namespace lvgl
