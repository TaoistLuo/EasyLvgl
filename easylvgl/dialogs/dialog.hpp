#pragma once
#include "lvgl/lvgl.h"
#include <functional>
#include <utility>

#include "easylvgl/core/event.hpp"

namespace easylvgl {

class LDialog {
private:
    lv_obj_t* msgbox_;

    explicit LDialog(lv_obj_t* obj) : msgbox_(obj) {}

public:
    LDialog() : msgbox_(nullptr) {}

    ~LDialog() {
        if (msgbox_) {
            lv_msgbox_close(msgbox_);
            msgbox_ = nullptr;
        }
    }

    LDialog(LDialog&& other) noexcept
        : msgbox_(other.msgbox_) {
        other.msgbox_ = nullptr;
    }

    LDialog& operator=(LDialog&& other) noexcept {
        if (this != &other) {
            if (msgbox_) {
                lv_msgbox_close(msgbox_);
            }
            msgbox_ = other.msgbox_;
            other.msgbox_ = nullptr;
        }
        return *this;
    }

    LDialog(const LDialog&) = delete;
    LDialog& operator=(const LDialog&) = delete;

    static LDialog create(const char* title, const char* msg) {
        LDialog d(lv_msgbox_create(nullptr));
        lv_msgbox_add_title(d.msgbox_, title);
        lv_msgbox_add_text(d.msgbox_, msg);
        return d;
    }

    static LDialog info(const char* title, const char* msg) {
        return create(title, msg);
    }

    static LDialog confirm(const char* title, const char* msg) {
        return create(title, msg);
    }

    LDialog& add_button(const char* text, std::function<void()> callback = nullptr) {
        if (msgbox_) {
            lv_obj_t* btn = lv_msgbox_add_footer_button(msgbox_, text);
            if (callback && btn) {
                on_clicked(btn, std::move(callback));
            }
        }
        return *this;
    }

    void show() {
        if (msgbox_) {
            lv_obj_move_foreground(msgbox_);
        }
    }

    void close() {
        if (msgbox_) {
            lv_msgbox_close(msgbox_);
            msgbox_ = nullptr;
        }
    }

    lv_obj_t* native() { return msgbox_; }
};

} // namespace easylvgl
