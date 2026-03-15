#pragma once
#include "lvgl/lvgl.h"
#include <functional>
#include <vector>

namespace lvgl {

class LDialog {
private:
    lv_obj_t* msgbox_;
    std::vector<std::function<void()>> callbacks_;
    
    LDialog() : msgbox_(nullptr) {}
    
public:
    static LDialog create(const char* title, const char* msg) {
        LDialog d;
        d.msgbox_ = lv_msgbox_create(nullptr);
        lv_msgbox_set_title(d.msgbox_, title);
        lv_msgbox_set_text(d.msgbox_, msg);
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
            lv_msgbox_add_close_button(msgbox_);
            if (callback) {
                callbacks_.push_back(callback);
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

} // namespace lvgl
