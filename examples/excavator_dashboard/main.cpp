/**************************************************************************************************
 * @file        main.cpp
 * @brief       工程级示例：挖掘机智能仪表盘（HMI）
 * @details     目标平台为桌面 Linux（SDL2 后端）。
 *              UI 由 luic 生成到构建目录（中间文件，不入工程），业务逻辑在
 * ui/screens/*.h/.cpp：
 *              - 工程清单 + 多屏幕 + 可复用组件 + 图片资源
 *              - 屏幕类继承 luic 生成的 *_UI，构造函数里 setup(parent) +
 * bind_events()
 *              - 多语言走 LVGL 原生 lv_translation：Excel
 * 文案源，构建期生成语言包
 *              - 运行时 Tiny TTF 加载完整中文字体（思源黑体同源）
 *
 * @author      taoist.luo
 * @version     1.5.0
 * @date        2026-11-08
 *
 * Copyright (c) 2026 taoist.luo (https://github.com/TaoistLuo/EasyLVGL)
 *
 * MIT License
 *************************************************************************************************/

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "app_common.hpp"
#include "easylvgl/core/platform.hpp"
#include "easylvgl/core/signal_handler.hpp"
#include "easylvgl/lvgl_widgets.hpp"
#include "ui/screens/Dashboard.h"
#include "ui/screens/InfoCenter.h"
#include "ui/screens/Settings.h"

using namespace easylvgl;
using namespace app;

#ifndef EXCAVATOR_FONT_FILE
#define EXCAVATOR_FONT_FILE "fonts/NotoSansSC-Regular.ttf"
#endif

namespace {

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

i18n::Lang lang_from_env() {
  const char* value = std::getenv("EASYLVGL_LANG");
  if (value &&
      (std::strcmp(value, "ru") == 0 || std::strcmp(value, "RU") == 0)) {
    return i18n::Lang::Ru;
  }
  if (value &&
      (std::strcmp(value, "en") == 0 || std::strcmp(value, "EN") == 0)) {
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

  i18n::init(lang_from_env());

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
    std::fprintf(
        stderr,
        "warning: font not loaded (%s), falling back to built-in fonts\n",
        EXCAVATOR_FONT_FILE);
  }

  SignalHandler signal_handler;

  lv_obj_t* page_dashboard = make_page(lv_scr_act(), font16);
  lv_obj_t* page_info = make_page(lv_scr_act(), font16);
  lv_obj_t* page_settings = make_page(lv_scr_act(), font16);

  Dashboard dashboard(page_dashboard);
  InfoCenter info(page_info);
  Settings settings(page_settings);

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
    std::printf("[LANG] %s\n", lang == i18n::Lang::En
                                   ? "en"
                                   : (lang == i18n::Lang::Ru ? "ru" : "zh"));
  };

  const uint32_t start_ms = static_cast<uint32_t>(
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count());
  const int auto_close_ms = env_int("EASYLVGL_AUTOCLOSE_MS", 0);
  uint32_t last_sample = 0;

  std::printf("============================================\n");
  std::printf("  EXCAVATOR HMI  (project-level example, Linux)\n");
  std::printf(
      "  ui: build/ui_generated (intermediate), logic: ui/screens/*.cpp\n");
  std::printf("  language: %s (LVGL lv_translation)\n",
              i18n::current() == i18n::Lang::En
                  ? "English"
                  : (i18n::current() == i18n::Lang::Ru ? "Русский" : "中文"));
  std::printf("  font: %s\n", EXCAVATOR_FONT_FILE);
  std::printf("  Press Ctrl+C to exit.\n");
  std::printf("============================================\n");

  while (signal_handler.is_running()) {
    const uint32_t now_ms =
        static_cast<uint32_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch())
                .count()) -
        start_ms;

    if (now_ms - last_sample >= 500u) {
      last_sample = now_ms;
      Telemetry t = simulate(now_ms);
      dashboard.apply(t, now_ms);
      info.apply(t, now_ms);
    }

    if (auto_close_ms > 0 && now_ms >= static_cast<uint32_t>(auto_close_ms)) {
      std::printf("[DEMO] auto close after %d ms (page %d)\n", auto_close_ms,
                  current_page);
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
