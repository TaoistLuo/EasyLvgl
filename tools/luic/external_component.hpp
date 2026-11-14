#pragma once

#include <map>
#include <string>

namespace luic {

/* 外部 C++ 组件：不生成 UI 头，直接实例化用户类。
 * 类需提供：Class(lv_obj_t* parent)、root()、set_pos/set_size/set_align，
 * 以及实例 props 对应的 set_<name>(...) 方法。 */
struct ExternalComponent {
    std::string include;
    std::string class_name;
};

using ExternalComponentMap = std::map<std::string, ExternalComponent>;

}  // namespace luic
