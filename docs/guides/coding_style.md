# LVGL++ 编码规范

## 概述

本文档定义了 LVGL++ 项目的 C++ 编码规范，确保代码风格统一、可读性强、易于维护。

---

## 1. 命名规范

### 1.1 类名（Class Names）

**规则：** 大驼峰命名法（PascalCase），以 `L` 开头（类似 Qt 风格）

```cpp
// ✅ 正确
class LButton { };
class LVBoxLayout { };
class LComboBox { };
class LLineEdit { };
class LWidget { };
class LStyle { };
class LImageButton { };

// ❌ 错误
class Button { };           // 缺少 L 前缀
class button { };           // 不应使用小写
class l_button { };         // 不应使用蛇形
class lButton { };          // L 应该是前缀，不是驼峰的一部分
```

> **注意：** 布局类使用具体描述名，如 `LVBoxLayout`、`LHBoxLayout`（不是 LVStack/LHStack）。
> 输入组件使用 `LLineEdit`（单行）和 `LTextEdit`（多行），下拉列表使用 `LComboBox`（不是 LDropdown）。

### 1.2 结构体名（Struct Names）

**规则：** 蛇形命名法（snake_case），以 `_t` 结尾

```cpp
// ✅ 正确
struct widget_config_t {
    int width;
    int height;
};

struct event_data_t {
    void* user_data;
    int event_code;
};

struct style_props_t {
    uint32_t bg_color;
    uint16_t radius;
};

// ❌ 错误
struct WidgetConfig { };      // 不应使用大驼峰
struct eventData { };         // 不应使用小驼峰
struct style_props { };       // 缺少 _t 后缀
```

### 1.3 枚举名（Enum Names）

**规则：** 蛇形命名法（snake_case），以 `_e` 结尾

```cpp
// ✅ 正确
enum button_state_e {
    BUTTON_STATE_NORMAL,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_DISABLED
};

enum align_mode_e {
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
};

// ❌ 错误
enum ButtonState { };         // 不应使用大驼峰
enum buttonState { };         // 不应使用小驼峰
enum button_state { };        // 缺少 _e 后缀
```

**枚举值：** 全大写，下划线分隔，带枚举名前缀

```cpp
// ✅ 正确
enum widget_type_e {
    WIDGET_TYPE_BUTTON,
    WIDGET_TYPE_LABEL,
    WIDGET_TYPE_SLIDER
};

// ❌ 错误
enum widget_type_e {
    Button,              // 不应使用大驼峰
    label,               // 不应使用小写
    TYPE_SLIDER          // 缺少完整前缀
};
```

### 1.4 变量名（Variable Names）

**规则：** 蛇形命名法（snake_case）

```cpp
// ✅ 正确
int button_count = 0;
std::string user_name;
bool is_enabled = true;
float scale_factor = 1.0f;

// ❌ 错误
int ButtonCount = 0;          // 不应使用大驼峰
std::string userName;         // 不应使用小驼峰
bool IsEnabled = true;        // 不应使用大驼峰
```

### 1.5 类成员变量（Class Member Variables）

**规则：** 蛇形命名法（snake_case），以 `m_` 开头

```cpp
// ✅ 正确
class LButton {
private:
    lv_obj_t* m_obj;
    lv_obj_t* m_label;
    int m_width;
    int m_height;
    bool m_is_pressed;
    std::string m_text;
};

// ❌ 错误
class LButton {
private:
    lv_obj_t* obj;           // 缺少 m_ 前缀
    lv_obj_t* _label;        // 不应使用 _ 前缀
    int Width;               // 不应使用大驼峰
    int mHeight;             // m 应该是前缀，不是驼峰的一部分
    bool isPressed;          // 缺少 m_ 前缀
};
```

**保护成员和公有成员：** 同样使用 `m_` 前缀

```cpp
class LWidget {
protected:
    lv_obj_t* m_obj;         // ✅ 保护成员
    
public:
    int m_user_data;         // ✅ 公有成员（不推荐，但如果需要）
};
```

### 1.6 函数名（Function Names）

**规则：** 蛇形命名法（snake_case），setter 使用 `set_` 前缀

```cpp
// ✅ 正确
void set_text(const char* text);
int get_width() const;
bool is_enabled() const;
void on_click_handler();
void bind_events();

// ❌ 错误
void SetText(const char* text);      // 不应使用大驼峰
int GetWidth() const;                // 不应使用大驼峰
bool IsEnabled() const;              // 不应使用大驼峰
```

### 1.7 常量名（Constant Names）

**规则：** 全大写，下划线分隔

```cpp
// ✅ 正确
const int MAX_BUFFER_SIZE = 1024;
const float PI = 3.14159f;
constexpr int DEFAULT_WIDTH = 100;

// ❌ 错误
const int maxBufferSize = 1024;      // 不应使用小驼峰
const float Pi = 3.14159f;           // 不应使用大驼峰
```

### 1.8 命名空间（Namespace Names）

**规则：** 全小写，可使用下划线

```cpp
// ✅ 正确
namespace lvgl {
namespace internal {
namespace detail {

// ❌ 错误
namespace LVGL { };                  // 不应全大写
namespace Lvgl { };                  // 不应使用大驼峰
```

### 1.9 宏定义（Macro Names）

**规则：** 全大写，下划线分隔，带项目前缀

```cpp
// ✅ 正确
#define EASYLVGL_VERSION "2.0.0"
#define EASYLVGL_MAX_WIDGETS 100
#define EASYLVGL_ENABLE_DEBUG 1

// ❌ 错误
#define version "2.0.0"              // 缺少前缀，不应小写
#define MaxWidgets 100               // 不应使用大驼峰
```

---

## 2. 代码格式

### 2.1 缩进

- 使用 **4 个空格** 缩进
- 不使用 Tab 字符

```cpp
// ✅ 正确
class LButton {
public:
    void set_text(const char* text) {
        if (text != nullptr) {
            m_text = text;
        }
    }
};
```

### 2.2 大括号

- 左大括号 `{` 不换行（K&R 风格）
- 右大括号 `}` 单独一行

```cpp
// ✅ 正确
if (condition) {
    do_something();
}

class LWidget {
public:
    void method() {
        // ...
    }
};

// ❌ 错误
if (condition)
{                    // 左大括号不应换行
    do_something();
}
```

### 2.3 空格

- 关键字后加空格：`if (`, `for (`, `while (`
- 运算符两侧加空格：`a + b`, `x = y`
- 逗号后加空格：`func(a, b, c)`
- 指针和引用符号靠近类型：`int* ptr`, `int& ref`

```cpp
// ✅ 正确
if (x > 0) {
    int* ptr = &x;
    int& ref = x;
    result = a + b * c;
    func(a, b, c);
}

// ❌ 错误
if(x>0){                 // 缺少空格
    int *ptr=&x;         // 空格位置错误
    result=a+b*c;        // 缺少空格
    func(a,b,c);         // 逗号后缺少空格
}
```

### 2.4 行长度

- 每行最多 **120 个字符**
- 超过时适当换行

```cpp
// ✅ 正确
btn.set_text("Click Me")
   .set_size(100, 50)
   .primary()
   .on_click([](auto* e) {
       printf("Clicked!\n");
   });

// ✅ 也可以（不超过 120 字符）
btn.set_text("Click Me").set_size(100, 50).primary();

// ❌ 错误（超过 120 字符）
btn.set_text("Very Long Text Here").set_size(100, 50).primary().secondary().on_click([](auto* e) { printf("Clicked!\n"); });
```

---

## 3. 注释规范

### 3.1 文件头注释

```cpp
/**
 * @file button.hpp
 * @brief Button component implementation
 * @author LVGL++ Team
 * @date 2026-03-15
 */
```

### 3.2 类注释

使用 Doxygen 格式：

```cpp
/**
 * @brief Button component
 * 
 * Wraps lv_button to provide a modern button with fluent API.
 * Supports multiple themes and event handling.
 * 
 * @example
 * LButton btn;
 * btn.set_text("Click Me")
 *    .set_size(100, 50)
 *    .primary()
 *    .on_click([](auto* e) {
 *        printf("Clicked!\n");
 *    });
 */
class LButton : public LWidget {
    // ...
};
```

### 3.3 函数注释

```cpp
/**
 * @brief Set button text
 * @param text Text content (null-terminated string)
 * @return LButton& Reference for chaining
 */
LButton& set_text(const char* text);
```

### 3.4 成员变量注释

```cpp
class LButton {
private:
    lv_obj_t* m_obj;      ///< Native LVGL object
    lv_obj_t* m_label;    ///< Button text label
    int m_width;           ///< Button width in pixels
    int m_height;          ///< Button height in pixels
};
```

### 3.5 行内注释

```cpp
// ✅ 正确：简洁说明
int count = 0;  // Widget counter

// ❌ 错误：过于冗长
int count = 0;  // This variable is used to count the number of widgets that have been created in the application
```

---

## 4. 类设计规范

### 4.1 继承

```cpp
// ✅ 正确
class LButton : public LWidget {
public:
    explicit LButton(lv_obj_t* parent = nullptr);
    ~LButton() = default;
};
```

### 4.2 构造函数

- 单参数构造函数使用 `explicit`
- 默认参数放在声明中

```cpp
// ✅ 正确
class LWidget {
public:
    explicit LWidget(lv_obj_t* obj);
    LWidget(int width, int height);
};

// ❌ 错误
class LWidget {
public:
    LWidget(lv_obj_t* obj);  // 缺少 explicit
};
```

### 4.3 访问控制

顺序：`public` → `protected` → `private`

```cpp
// ✅ 正确
class LButton {
public:
    LButton();
    void set_text(const char* text);
    
protected:
    void update_layout();
    
private:
    lv_obj_t* m_obj;
    lv_obj_t* m_label;
};
```

### 4.4 方法链式调用

返回引用支持链式调用：

```cpp
// ✅ 正确
class LButton {
public:
    LButton& set_text(const char* text) {
        // ...
        return *this;
    }
    
    LButton& set_size(int w, int h) {
        // ...
        return *this;
    }
};
```

---

## 5. 头文件规范

### 5.1 Include Guard

使用 `#pragma once`：

```cpp
// ✅ 正确
#pragma once

#include "../core/widget.hpp"

namespace lvgl {
    // ...
}
```

### 5.2 Include 顺序

1. 相关头文件
2. C++ 标准库
3. 第三方库
4. 项目头文件

```cpp
// ✅ 正确
#pragma once

// 相关头文件
#include "../core/widget.hpp"
#include "../core/style.hpp"

// C++ 标准库
#include <string>
#include <vector>

// 第三方库
#include "lvgl/lvgl.h"

// 项目头文件
#include "button_theme.hpp"
```

---

## 6. 示例代码

### 完整示例：LButton 类

```cpp
#pragma once
#include "../core/widget.hpp"
#include "../core/style.hpp"
#include "../core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @brief Button component
 * 
 * Wraps lv_button to provide a modern button with fluent API.
 */
class LButton : public LWidget {
private:
    lv_obj_t* m_label;    ///< Button text label
    
public:
    /**
     * @brief Constructor
     * @param parent Parent object (nullptr = current screen)
     */
    explicit LButton(lv_obj_t* parent = nullptr) 
        : LWidget(lv_button_create(parent ? parent : lv_scr_act())) {
        m_label = lv_label_create(m_obj);
        lv_obj_center(m_label);
    }
    
    /**
     * @brief Set button text
     * @param text Text content
     * @return LButton& Reference for chaining
     */
    LButton& set_text(const char* text) {
        lv_label_set_text(m_label, text);
        return *this;
    }
    
    /**
     * @brief Set button size
     * @param width Width in pixels
     * @param height Height in pixels
     * @return LButton& Reference for chaining
     */
    LButton& set_size(int width, int height) {
        lv_obj_set_size(m_obj, width, height);
        return *this;
    }
    
    /**
     * @brief Apply primary theme
     * @return LButton& Reference for chaining
     */
    LButton& primary() {
        LStyle style;
        style.set_bg_color(0x2196F3).apply_to(m_obj);
        return *this;
    }
    
    /**
     * @brief Register click event callback
     * @param callback Event callback function
     * @return LButton& Reference for chaining
     */
    template<typename Func>
    LButton& on_click(Func&& callback) {
        connect_event(m_obj, LV_EVENT_CLICKED, std::forward<Func>(callback));
        return *this;
    }
};

} // namespace lvgl
```

---

## 7. 命名规范总结表

| 类型 | 规则 | 示例 |
|------|------|------|
| 类名 | `L` + 大驼峰 | `LButton`, `LVBoxLayout`, `LComboBox` |
| 结构体名 | 蛇形 + `_t` | `widget_config_t`, `event_data_t` |
| 枚举名 | 蛇形 + `_e` | `button_state_e`, `align_mode_e` |
| 枚举值 | 全大写 + 前缀 | `BUTTON_STATE_NORMAL` |
| 变量名 | 蛇形 | `button_count`, `user_name` |
| 成员变量 | `m_` + 蛇形 | `m_obj`, `m_width`, `m_is_enabled` |
| 函数名 | 蛇形 | `set_text()`, `get_width()`, `bind_events()` |
| 常量名 | 全大写 | `MAX_SIZE`, `DEFAULT_WIDTH` |
| 命名空间 | 全小写 | `lvgl`, `internal` |
| 宏定义 | 全大写 + 前缀 | `EASYLVGL_VERSION` |

> **关于布局类名：** 早期设计使用 `LVStack`/`LHStack`，实际实现中使用更具描述性的 `LVBoxLayout`/`LHBoxLayout`。
> 两者都遵循 `L` + PascalCase 的命名规则。

---

## 8. 检查清单

在提交代码前，请确认：

- [ ] 类名使用 `L` + 大驼峰（如 `LButton`、`LVBoxLayout`）
- [ ] 结构体名使用蛇形 + `_t`
- [ ] 枚举名使用蛇形 + `_e`
- [ ] 变量名使用蛇形
- [ ] 成员变量使用 `m_` + 蛇形
- [ ] 函数名使用蛇形，setter 用 `set_` 前缀
- [ ] 使用 4 空格缩进
- [ ] 左大括号不换行
- [ ] 每行不超过 120 字符
- [ ] 添加了 Doxygen 注释
- [ ] 使用 `#pragma once`
- [ ] 单参数构造函数使用 `explicit`

---

## 9. 工具配置

### 9.1 clang-format 配置

创建 `.clang-format` 文件：

```yaml
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 120
PointerAlignment: Left
```

### 9.2 编辑器配置

创建 `.editorconfig` 文件：

```ini
[*.{cpp,hpp}]
indent_style = space
indent_size = 4
end_of_line = lf
charset = utf-8
trim_trailing_whitespace = true
insert_final_newline = true
```

---

**版本：** 2.0  
**日期：** 2026-03-15  
**状态：** 正式发布
