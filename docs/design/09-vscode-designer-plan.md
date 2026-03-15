# LVGL++ VSCode Designer 完整方案

**版本：** 1.0.0
**日期：** 2026-03-15
**核心理念：** .lui 文件 → 编译生成 C++ 代码 → 零运行时开销

---

## 1. 核心思路

### 对标方案对比

| 方案 | 描述文件 | 产物 | 运行时开销 | 代表 |
|------|---------|------|-----------|------|
| **Qt .ui** | XML | ui_xxx.h（C++代码） | **零** | Qt Designer |
| QML | QML | 解释执行 | 高 | Qt Quick |
| Android XML | XML | 运行时inflate | 中 | Android Studio |
| **本方案** | **.lui (JSON)** | **xxx_ui.hpp（C++代码）** | **零** | LVGL++ Designer |

### 我们的方案：编译时生成

```
开发阶段：
  VSCode 插件（可视化编辑）
       ↓ 保存
  main_screen.lui（JSON 描述文件）
       ↓ luic 编译器（构建时自动执行）
  main_screen_ui.hpp（纯 C++ 代码）
       ↓ #include
  你的业务代码（main.cpp）
       ↓ g++ 编译
  可执行文件（零运行时开销）
```

**关键点：.lui 文件只在开发阶段使用，最终产物是纯 C++ 代码，和手写代码完全一样。**

---

## 2. 文件格式设计

### 2.1 .lui 文件格式（JSON）

```json
{
  "version": "1.0",
  "name": "MainScreen",
  "size": [800, 480],
  "children": [
    {
      "type": "LLabel",
      "id": "title",
      "props": {
        "text": "Dashboard",
        "color": "0x2196F3",
        "font_size": 24
      }
    },
    {
      "type": "LButton",
      "id": "btn_login",
      "props": {
        "text": "Login",
        "size": [120, 50],
        "theme": "primary"
      },
      "events": ["clicked"]
    },
    {
      "type": "LVStack",
      "id": "info_panel",
      "props": {
        "spacing": 10,
        "padding": 15
      },
      "children": [
        {
          "type": "LSlider",
          "id": "brightness",
          "props": {
            "range": [0, 100],
            "value": 75
          },
          "events": ["value_changed"]
        },
        {
          "type": "LBar",
          "id": "progress",
          "props": {
            "range": [0, 100],
            "value": 50
          }
        }
      ]
    }
  ]
}
```

### 2.2 生成的 C++ 代码（main_screen_ui.hpp）

```cpp
#pragma once
#include "lvgl++/lvgl_widgets.hpp"

namespace lvgl {

/**
 * Auto-generated from main_screen.lui
 * DO NOT EDIT - Changes will be overwritten
 */
class MainScreen {
public:
    // 组件成员（可直接访问）
    LLabel*   title;
    LButton*  btn_login;
    LVStack*  info_panel;
    LSlider*  brightness;
    LBar*     progress;

    void setup(lv_obj_t* parent = nullptr) {
        lv_obj_t* root = parent ? parent : lv_scr_act();

        title = new LLabel(root);
        title->text("Dashboard")
              .color(0x2196F3)
              .font_size(24);

        btn_login = new LButton(root);
        btn_login->text("Login")
                  .size(120, 50)
                  .primary();

        info_panel = new LVStack(root);
        info_panel->spacing(10)
                   .padding(15);

        brightness = new LSlider(info_panel->native());
        brightness->range(0, 100)
                   .value(75);

        progress = new LBar(info_panel->native());
        progress->range(0, 100)
                 .value(50);
    }

    // 事件槽函数（用户实现）
    virtual void on_btn_login_clicked(lv_event_t* e) {}
    virtual void on_brightness_value_changed(lv_event_t* e) {}

    void bindEvents() {
        btn_login->on_click([this](lv_event_t* e) {
            on_btn_login_clicked(e);
        });
        brightness->on_change([this](lv_event_t* e) {
            on_brightness_value_changed(e);
        });
    }

    ~MainScreen() {
        delete title;
        delete btn_login;
        delete info_panel;
        delete brightness;
        delete progress;
    }
};

} // namespace lvgl
```

### 2.3 用户业务代码（main.cpp）

```cpp
#include "main_screen_ui.hpp"

class MyMainScreen : public lvgl::MainScreen {
public:
    void on_btn_login_clicked(lv_event_t* e) override {
        printf("Login clicked!\n");
    }

    void on_brightness_value_changed(lv_event_t* e) override {
        int val = brightness->get_value();
        printf("Brightness: %d\n", val);
    }
};

int main() {
    lv_init();
    // ...

    MyMainScreen screen;
    screen.setup();
    screen.bindEvents();

    while(signal_handler.is_running()) {
        lv_timer_handler();
        usleep(5000);
    }
    return 0;
}
```

---

## 3. luic 编译器设计

### 3.1 编译器架构

```
main_screen.lui
     ↓
[1. 解析] JSON Parser
     ↓
[2. 验证] Schema Validator（检查类型、属性合法性）
     ↓
[3. 构建] Widget Tree Builder（构建组件树）
     ↓
[4. 生成] C++ Code Generator（生成 .hpp 文件）
     ↓
main_screen_ui.hpp
```

### 3.2 技术选型

**方案 A：Python 脚本（推荐，快速实现）**
- 依赖少，跨平台
- 开发速度快
- 易于集成到 CMake

**方案 B：C++ 工具**
- 性能好
- 但开发周期长

**建议：先用 Python 实现 MVP，后续按需用 C++ 重写。**

### 3.3 CMake 集成

```cmake
# CMakeLists.txt 中自动编译 .lui 文件
file(GLOB LUI_FILES "${CMAKE_SOURCE_DIR}/ui/*.lui")

foreach(LUI_FILE ${LUI_FILES})
    get_filename_component(LUI_NAME ${LUI_FILE} NAME_WE)
    set(LUI_OUTPUT "${CMAKE_SOURCE_DIR}/ui_generated/${LUI_NAME}_ui.hpp")

    add_custom_command(
        OUTPUT ${LUI_OUTPUT}
        COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/luic.py
                ${LUI_FILE} -o ${LUI_OUTPUT}
        DEPENDS ${LUI_FILE}
        COMMENT "Compiling ${LUI_NAME}.lui -> ${LUI_NAME}_ui.hpp"
    )
    list(APPEND LUI_GENERATED ${LUI_OUTPUT})
endforeach()

add_custom_target(lui_compile ALL DEPENDS ${LUI_GENERATED})
```

**效果：修改 .lui 文件后，make 自动重新生成 C++ 代码。**

---

## 4. VSCode 插件设计

### 4.1 插件功能

```
┌─────────────────────────────────────────────────────────────┐
│ VSCode                                                       │
│ ┌──────────┬────────────────────────┬──────────────────────┐│
│ │ 组件面板 │      画布区域          │    属性面板          ││
│ │          │                        │                      ││
│ │ [Button] │  ┌──────────────────┐  │ ID: btn_login        ││
│ │ [Label]  │  │  Dashboard       │  │ Text: Login          ││
│ │ [Slider] │  │                  │  │ Size: 120 x 50       ││
│ │ [Bar]    │  │  [Login]         │  │ Theme: primary       ││
│ │ [Arc]    │  │                  │  │                      ││
│ │ [Switch] │  │  ═══════ 75%     │  │ Events:              ││
│ │ [Table]  │  │  ████████░░ 50%  │  │  ☑ clicked           ││
│ │ ...      │  │                  │  │  ☐ pressed           ││
│ │          │  └──────────────────┘  │  ☐ released          ││
│ └──────────┴────────────────────────┴──────────────────────┘│
│ ┌──────────────────────────────────────────────────────────┐│
│ │ 生成的代码预览（只读）                                    ││
│ │ class MainScreen {                                        ││
│ │     LButton* btn_login;                                   ││
│ │     ...                                                   ││
│ │ };                                                        ││
│ └──────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────┘
```

### 4.2 核心功能清单

**P0（必须）：**
1. .lui 文件的可视化编辑器（Webview Panel）
2. 组件拖拽到画布
3. 属性面板编辑
4. 保存时自动生成 C++ 代码
5. 生成代码实时预览

**P1（重要）：**
6. 组件树视图（TreeView）
7. 对齐辅助线
8. 撤销/重做
9. 复制/粘贴组件
10. 键盘快捷键

**P2（增强）：**
11. 主题切换预览
12. 多屏幕管理
13. 自定义组件注册
14. 代码片段生成
15. 与 LVGL 模拟器联动预览

### 4.3 插件技术栈

```
VSCode Extension
├── extension.ts          # 插件入口
├── LuiEditorProvider.ts  # 自定义编辑器
├── webview/              # Webview 前端
│   ├── index.html
│   ├── canvas.ts         # 画布渲染（Canvas API / SVG）
│   ├── properties.ts     # 属性面板
│   ├── components.ts     # 组件面板
│   └── codegen.ts        # 代码生成预览
├── luic/                 # 编译器核心
│   ├── parser.ts         # .lui 解析
│   ├── validator.ts      # 格式验证
│   └── generator.ts      # C++ 代码生成
└── schemas/
    └── lui.schema.json   # JSON Schema（智能提示）
```

### 4.4 工作流程

```
1. 用户打开 .lui 文件
   → VSCode 自动启动可视化编辑器

2. 用户拖拽组件到画布
   → 实时更新 .lui JSON
   → 实时更新代码预览

3. 用户编辑属性
   → 画布实时刷新
   → .lui 文件实时更新

4. 用户保存（Ctrl+S）
   → 自动运行 luic 编译器
   → 生成 xxx_ui.hpp
   → 触发 C++ 编译（如果配置了）

5. 用户也可以直接编辑 .lui JSON
   → 画布自动同步更新（双向编辑）
```

---

## 5. 完整开发流程（用户视角）

### 5.1 创建新界面

```bash
# 1. 在 ui/ 目录下创建 .lui 文件
touch ui/settings_screen.lui

# 2. VSCode 自动打开可视化编辑器
# 3. 拖拽组件，编辑属性
# 4. 保存 → 自动生成 settings_screen_ui.hpp
```

### 5.2 编写业务逻辑

```cpp
// settings_screen.cpp
#include "ui_generated/settings_screen_ui.hpp"

class SettingsScreen : public lvgl::SettingsScreen_UI {
public:
    void on_btn_save_clicked(lv_event_t* e) override {
        save_settings();
    }

    void on_slider_brightness_value_changed(lv_event_t* e) override {
        int val = slider_brightness->get_value();
        set_brightness(val);
    }

private:
    void save_settings() { /* ... */ }
    void set_brightness(int val) { /* ... */ }
};
```

### 5.3 编译运行

```bash
cd build
cmake ..    # 自动检测 .lui 文件变化，运行 luic
make -j4    # 编译 C++ 代码
./bin/app   # 运行，零运行时开销
```

---

## 6. 项目结构

```
lv_port_linux/
├── lvgl++/                    # C++ 封装库（已完成）
│   ├── core/
│   ├── widgets/
│   └── ...
│
├── tools/                     # 工具链
│   └── luic.py                # .lui → C++ 编译器
│
├── ui/                        # .lui 界面描述文件
│   ├── main_screen.lui
│   └── settings_screen.lui
│
├── ui_generated/              # 自动生成的 C++ 代码（不要手动编辑）
│   ├── main_screen_ui.hpp
│   └── settings_screen_ui.hpp
│
├── src/                       # 用户业务代码
│   ├── main.cpp
│   ├── main_screen.cpp        # 继承 MainScreen_UI，实现事件
│   └── settings_screen.cpp
│
├── vscode-lvgl-designer/      # VSCode 插件源码
│   ├── package.json
│   ├── src/
│   │   ├── extension.ts
│   │   ├── LuiEditorProvider.ts
│   │   └── webview/
│   └── schemas/
│       └── lui.schema.json
│
├── examples/                  # 示例
├── docs/                      # 文档
└── CMakeLists.txt
```

---

## 7. 实施路线图

### 阶段 1：luic 编译器（1周）
- [ ] 实现 .lui JSON 解析
- [ ] 实现 C++ 代码生成器
- [ ] CMake 集成
- [ ] 第一个端到端示例

### 阶段 2：VSCode 插件 MVP（2-3周）
- [ ] 插件脚手架
- [ ] 自定义编辑器（CustomEditor API）
- [ ] 画布渲染（5-10 个常用组件）
- [ ] 属性面板
- [ ] 保存时自动编译

### 阶段 3：完善插件（2-3周）
- [ ] 拖拽交互
- [ ] 对齐辅助线
- [ ] 撤销/重做
- [ ] 组件树视图
- [ ] 所有 32 个组件支持

### 阶段 4：高级功能（2-4周）
- [ ] 多屏幕管理
- [ ] 主题预览
- [ ] 自定义组件
- [ ] 与 LVGL 模拟器联动
- [ ] 发布到 VSCode Marketplace

---

## 8. 优势总结

### 对比 QML/JSON 运行时加载

| 特性 | 本方案（编译时生成） | QML/JSON 运行时加载 |
|------|---------------------|-------------------|
| **运行时开销** | **零** | 解析+构建开销 |
| **启动速度** | **极快** | 需要解析时间 |
| **内存占用** | **最小** | 需要解析器内存 |
| **类型安全** | **编译时检查** | 运行时才发现错误 |
| **调试** | **源码级调试** | 难以调试 |
| **嵌入式友好** | **完全适合** | 资源受限时不适合 |
| **代码大小** | **最小** | 需要额外的解析库 |

### 对比手写 C++ 代码

| 特性 | 本方案 | 手写 C++ |
|------|--------|---------|
| **开发速度** | **快 3-5 倍** | 慢 |
| **可视化** | **所见即所得** | 脑补 |
| **错误率** | **低** | 容易出错 |
| **团队协作** | **设计师可参与** | 只有开发者 |
| **运行性能** | **完全相同** | 基准 |

---

## 9. 风险和应对

| 风险 | 影响 | 应对 |
|------|------|------|
| VSCode 插件开发复杂 | 中 | 先做 luic 编译器，手写 .lui 也能用 |
| 组件属性映射不完整 | 低 | 渐进式支持，先覆盖常用属性 |
| 生成代码质量 | 中 | 参考 Qt uic 的成熟方案 |
| 画布渲染与实际效果差异 | 中 | 提供模拟器联动预览 |

---

## 10. 下一步行动

### 最小可行路径（MVP）

**第 1 步：luic 编译器（3-5天）**
- Python 脚本
- 支持 5 个常用组件
- 生成可编译的 C++ 代码
- CMake 集成

**第 2 步：手写 .lui 验证（1-2天）**
- 创建示例 .lui 文件
- 编译生成 C++ 代码
- 编译运行验证

**第 3 步：VSCode 插件原型（1-2周）**
- 基础可视化编辑
- 保存时自动编译
- 代码预览

**总计 MVP：2-3 周**
