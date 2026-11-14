#pragma once

/*
 * 多语言封装：底层使用 LVGL 原生 lv_translation。
 * 文案源是 i18n/strings.xlsx，构建时由 tools/xlsx2i18n.py 生成 i18n_generated.hpp，
 * 本文件只负责注册语言包、切换语言，以及给控件绑定“语言变化时自动重译”。
 */

#include "lvgl/lvgl.h"
#include "i18n_generated.hpp"

#include <cstring>
#include <functional>
#include <utility>

namespace i18n {

enum class Lang { Zh = 0, En = 1, Ru = 2 };

inline const char* tr(exc::K key) {
    return lv_tr(exc::tag(key));
}

inline void set_language(Lang lang) {
    lv_translation_set_language(exc::kLanguages[static_cast<int>(lang)]);
}

inline Lang current() {
    const char* language = lv_translation_get_language();
    if (language && std::strcmp(language, "ru") == 0) return Lang::Ru;
    if (language && std::strcmp(language, "en") == 0) return Lang::En;
    return Lang::Zh;
}

inline void init(Lang lang) {
    lv_translation_add_static(exc::kLanguages, exc::kTags, exc::kTranslations);
    set_language(lang);
}

inline void bind_label(lv_obj_t* label, exc::K key) {
    lv_label_set_translation_tag(label, exc::tag(key));
}

namespace detail {

struct TextBinding {
    exc::K key;
    std::function<void(const char*)> apply;
};

inline void text_language_cb(lv_event_t* e) {
    auto* binding = static_cast<TextBinding*>(lv_event_get_user_data(e));
    binding->apply(lv_tr(exc::tag(binding->key)));
}

inline void text_delete_cb(lv_event_t* e) {
    delete static_cast<TextBinding*>(lv_event_get_user_data(e));
}

inline void hook_language_cb(lv_event_t* e) {
    (*static_cast<std::function<void()>*>(lv_event_get_user_data(e)))();
}

inline void hook_delete_cb(lv_event_t* e) {
    delete static_cast<std::function<void()>*>(lv_event_get_user_data(e));
}

}  // namespace detail

/* 非 LLabel 控件（按钮、单选、复选、输入框、组件导出参数）没有 tag API，
 * 用语言变化事件重新调用 setter；创建时也会立即应用一次。 */
inline void bind_text(lv_obj_t* obj, exc::K key, std::function<void(const char*)> apply) {
    auto* binding = new detail::TextBinding{key, std::move(apply)};
    lv_obj_add_event_cb(obj, detail::text_language_cb, LV_EVENT_TRANSLATION_LANGUAGE_CHANGED, binding);
    lv_obj_add_event_cb(obj, detail::text_delete_cb, LV_EVENT_DELETE, binding);
    binding->apply(lv_tr(exc::tag(key)));
}

/* 语言变化时需要重新计算的动态文本（模式名、反馈、实时状态词）。 */
inline void on_language_change(lv_obj_t* obj, std::function<void()> refresh) {
    auto* hook = new std::function<void()>(std::move(refresh));
    lv_obj_add_event_cb(obj, detail::hook_language_cb, LV_EVENT_TRANSLATION_LANGUAGE_CHANGED, hook);
    lv_obj_add_event_cb(obj, detail::hook_delete_cb, LV_EVENT_DELETE, hook);
}

}  // namespace i18n
