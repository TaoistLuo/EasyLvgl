#pragma once

#include "easylvgl/lvgl_widgets.hpp"

class TempRow {
 public:
  explicit TempRow(lv_obj_t* parent = nullptr)
      : m_root(new easylvgl::LObj(parent)),
        m_label(new easylvgl::LLabel(m_root->native())),
        m_value(new easylvgl::LLabel(m_root->native())) {
    m_label->set_pos(0, 2).set_size(120, 22).set_color(0x8FA0B3);
    m_value->set_pos(126, 2).set_size(94, 22).set_color(0xE8EEF5);
    lv_obj_set_style_text_align(m_value->native(), LV_TEXT_ALIGN_RIGHT, 0);
  }

  ~TempRow() {
    delete m_value;
    delete m_label;
    delete m_root;
  }

  lv_obj_t* root() const { return m_root->native(); }
  TempRow& set_pos(int x, int y) {
    m_root->set_pos(x, y);
    return *this;
  }
  TempRow& set_size(int w, int h) {
    m_root->set_size(w, h);
    return *this;
  }
  TempRow& set_align(lv_align_t a) {
    m_root->set_align(a);
    return *this;
  }
  TempRow& set_label(const char* text) {
    m_label->set_text(text);
    return *this;
  }
  TempRow& set_value(const char* text) {
    m_value->set_text(text);
    return *this;
  }

 private:
  easylvgl::LObj* m_root;
  easylvgl::LLabel* m_label;
  easylvgl::LLabel* m_value;
};
