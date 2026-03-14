#pragma once

/**
 * @file lvgl_core.hpp
 * @brief LVGL++ Layer 1统一入口
 * @version 1.0.0
 * @date 2026-03-15
 */

#include "core/widget.hpp"
#include "core/style.hpp"
#include "core/event.hpp"
#include "core/config.hpp"

namespace lvgl {

constexpr int VERSION_MAJOR = 1;
constexpr int VERSION_MINOR = 0;
constexpr int VERSION_PATCH = 0;

inline ObjectConfig& ObjectConfig::style(LStyle& s, lv_style_selector_t selector) {
    s.apply_to(m_obj, selector);
    return *this;
}

} // namespace lvgl
