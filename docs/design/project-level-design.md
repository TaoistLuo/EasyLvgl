# LVGL++ 工程级设计（组件化 + 工程管理）

**版本：** 0.1（草案，待评审）
**状态：** 📝 讨论稿 —— 未实现，未改任何代码
**关联文档：** [json-ui-format.md](json-ui-format.md)（`.lui` 现有格式）、[architecture-design.md](architecture-design.md)、[DEVELOPMENT_GUIDELINES.md](../../DEVELOPMENT_GUIDELINES.md)

---

## 1. 背景与目标

LVGL++ 的目标是**让 LVGL 开发像 Qt 一样简单**。目前的工具链（luic 编译器 + Qt 设计器）已经能覆盖单个 `.lui` 文件的完整流程，问题出在**工程级**：

- 没有"工程"这个概念 —— 一次只能处理一个文件
- 没有自定义组件 —— 同样的按钮组/卡片/菜单块只能复制粘贴
- 因此规模稍大的项目就不好组织

本文档定义工程级方案：**组件化**（复用）与**工程管理**（组织与构建）。

---

## 2. 现状与缺口（已核实）

### 2.1 C++ 层其实已经具备复用能力

luic 生成的就是普通可复用类（`tools/luic/codegen.hpp:288`）：

```cpp
class Button_UI {
protected:
    LButton* m_w = nullptr;
public:
    static constexpr int kScreenWidth = 400;
    static constexpr int kScreenHeight = 200;
    void setup(lv_obj_t* parent = nullptr);
};
```

所以手写代码里早就可以 `Button_UI b; b.setup(someContainer);` 组合。**底座是对的。**

### 2.2 缺口在格式 / 编译器 / 设计器三层

| 层 | 现状 | 证据 |
|---|---|---|
| `.lui` 格式 | 无 include / import / 引用字段 | `tools/luic/ir.hpp`、`parser.hpp` 无相关字段 |
| luic | 只 `#include "lvgl++/lvgl_widgets.hpp"`，从不 include 其他生成头 | `tools/luic/codegen.hpp:274` |
| 组件注册表 | 只有 43 个内置 LVGL 组件 | `tools/luic/component_registry.hpp` |
| 设计器 | 组件面板只有内置组件，无工程概念 | `tools/lvgl-designer/src/panels/ComponentPanel.hpp` |

### 2.3 根因：文件 = 屏幕

每个 `.lui` 都**隐含是一整块屏幕**：

- 生成类带 `kScreenWidth/kScreenHeight`，运行时靠它建窗口（`examples/widgets_demo.cpp` 里 `lv_sdl_window_create(UI::kScreenWidth, UI::kScreenHeight)`）
- `setup(parent = nullptr)` 默认挂到 `lv_scr_act()`
- 设计器把内容直接建在 `lv_scr_act()` 上（`LvglRenderer::buildTree`）
- 主题的 `disp_size` 由这块"屏幕"的尺寸决定

**所以无法复用**：一个文件不是一个**部件**，而是一个**应用**。

> 这是本方案要解决的核心问题，其余都是它的推论。

---

## 3. 核心概念

| 概念 | 定义 | 类比 |
|---|---|---|
| **部件** Component | 一段可复用 UI。无屏幕语义：不带尺寸、不碰 `lv_scr_act()`、不参与 `disp_size` | QML 的 `.qml` 文件 / Qt 的 QWidget 子类 |
| **屏幕** Screen | 一种**特殊部件**：带尺寸、决定主题档位、是运行时入口 | QML 的 `Window` / Qt 的 `QMainWindow` |
| **实例** Instance | 部件在父文件里的一次使用 | `MyButton { text: "确定" }` |
| **导出参数** Export | 部件对外暴露的可调项，映射到内部某元素的某属性 | QML 的 `property` |
| **工程** Project | 一组 `.lui` + 入口屏幕 + 构建信息 | Qt 的 `.pro` / `.pri` |

设计原则：**文件 = 部件，屏幕是特例**（已确认）。

---

## 4. `.lui` 格式扩展

### 4.1 根节点新增 `kind`

```json
{ "name": "MainScreen", "kind": "screen", "size": [400, 320], "children": [ ... ] }
{ "name": "MyCard",     "kind": "component", "children": [ ... ] }
```

- `kind` 省略时**默认 `"screen"`** —— 现有 38 个示例全部无需修改，天然兼容
- `kind: "screen"` 才允许有 `size`，才生成 `kScreenWidth/kScreenHeight`
- `kind: "component"` **不允许**有 `size`（luic 报错）

### 4.2 组件新增 `exports`

```json
{
  "name": "MyCard",
  "kind": "component",
  "exports": {
    "caption": "m_title.text",
    "accent":  "m_bar.bg_color"
  },
  "children": [
    { "id": "m_title", "type": "LLabel", "props": { "text": "默认标题" } },
    { "id": "m_bar",   "type": "LBar",   "props": { "value": 50 } }
  ]
}
```

读法：`"caption": "m_title.text"` = 外部参数 `caption` 落到内部 `m_title` 标签的 `text` 属性。

**这就是 QML `property` 的角色**，但实现为**普通 C++ setter**，编译期检查，不需要 moc / 反射 / 运行时字符串查找。

### 4.3 新增实例语法 `"type": "@Name"`

父文件里嵌入一个组件实例：

```json
{
  "name": "MainScreen",
  "kind": "screen",
  "size": [400, 320],
  "children": [
    { "id": "card1", "type": "@MyCard",
      "props": { "pos": [10, 10], "size": [180, 120],
                 "caption": "第一张", "accent": "0xFF5722" } },
    { "id": "card2", "type": "@MyCard",
      "props": { "pos": [200, 10], "size": [180, 120],
                 "caption": "第二张" } }
  ]
}
```

- `pos` / `size` 作用在实例的**根容器**上
- 其余键按 `exports` 解析为参数；**未在 `exports` 中声明的键报错**（防止拼写错误静默失效）

---

## 5. 代码生成规范

### 5.1 组件类：必须有独立根容器

现状是"把子节点直接挂到 `parent`"，这对可复用组件不成立 —— 父级的 flex/grid 会打乱组件内部布局，实例也无法整体移动/设尺寸。

改为：

```cpp
// 由 MyCard.lui 生成
class MyCard_UI {
protected:
    LObj* m_root = nullptr;       // 组件自己的根容器
    LLabel* m_m_title = nullptr;
    LBar*   m_m_bar   = nullptr;

public:
    void setup(lv_obj_t* parent = nullptr) {
        m_root = new LObj(parent);
        m_root->set_size(LV_SIZE_CONTENT, LV_SIZE_CONTENT);   // 默认自适应内容
        m_root->set_layout_vbox();                            // 组件根独占布局

        m_m_title = new LLabel(m_root->native());
        m_m_title->set_text("默认标题");
        m_m_bar = new LBar(m_root->native());
        m_m_bar->set_value(50);
    }

    /* 每个导出项生成一个 setter */
    MyCard_UI& set_caption(const char* v) { m_m_title->set_text(v);   return *this; }
    MyCard_UI& set_accent(uint32_t v)     { m_m_bar->set_bg_color(v); return *this; }

    lv_obj_t* root() const { return m_root->native(); }

    virtual ~MyCard_UI() { delete m_root; }   // 删除根容器即整棵子树
};
```

要点：
- **内部成员仍是 `m_<内部id>`** —— 因为每个组件是独立作用域，天然不冲突。这正是根容器带来的好处
- 组件根**独占布局**（父级的布局不会窜进组件内部）
- 默认 `LV_SIZE_CONTENT`（自适应内容），实例上可覆盖

### 5.2 屏幕类：保持现状

`kind: "screen"` 的生成代码与今天一致（带 `kScreenWidth/kScreenHeight`，子节点直接挂 `lv_scr_act()`），保证向后兼容。

### 5.3 实例化与参数应用

```cpp
// 由 MainScreen.lui 生成
#include "my_card_ui.hpp"

class MainScreen_UI {
protected:
    MyCard_UI* m_card1 = nullptr;
    MyCard_UI* m_card2 = nullptr;

public:
    void setup(lv_obj_t* parent = nullptr) {
        lv_obj_t* root = parent ? parent : lv_scr_act();

        m_card1 = new MyCard_UI();
        m_card1->setup(root);
        m_card1->set_pos(10, 10)
               ->set_size(180, 120);
        m_card1->set_caption("第一张");
        m_card1->set_accent(0xFF5722);

        m_card2 = new MyCard_UI();
        m_card2->setup(root);
        m_card2->set_pos(200, 10)
               ->set_size(180, 120);
        m_card2->set_caption("第二张");
    }

    virtual ~MainScreen_UI() { delete m_card1; delete m_card2; }
};
```

### 5.4 属性类型 → setter 参数类型的映射

沿用现有属性类型系统（`component_registry.hpp`）：

| 属性类型 | C++ 参数类型 |
|---|---|
| `int` | `int` |
| `int_pair`（size/pos 等） | 展开为 `set_x(int,int)`；或保留 `int` 两个 setter |
| `string` | `const char*` |
| `hex`（颜色） | `uint32_t` |
| `bool` | `bool` |
| `enum` | `int`（或生成 enum 常量） |

> 细节待定：`int_pair` 类的导出是生成 `set_size(int,int)` 还是一个 `set_size(int,int)` 直接透传。倾向后者（直观）。

### 5.5 include 与文件命名

- 生成头命名沿用现状：`<snake_case(name)>_ui.hpp`（如 `my_card_ui.hpp`）
- 父文件生成头**只 include 它真正用到的组件头**（按依赖关系），无需手工维护顺序
- 组件头之间不得互相 include 形成环 —— 见 §6.3 循环检测

---

## 6. 工程管理与 luic 工程模式

### 6.1 工程清单（`.pri` 式薄清单）

一个 JSON 清单，**唯一职责是"本工程包含哪些 `.lui`、入口是哪个"**：

```json
{
  "version": "1.0",
  "name": "MyProject",
  "entry": "MainScreen",
  "includeDirs": ["ui/components", "ui/screens"],
  "files": [
    "ui/components/MyCard.lui",
    "ui/screens/MainScreen.lui"
  ]
}
```

设计要点：**不重复信息** —— 谁是组件谁是屏幕由各 `.lui` 自己的 `kind` 决定，清单不写第二遍。

### 6.2 生成目录约定

- luic 输出到 `ui_generated/`（沿用现状）
- 保持目录结构镜像（`ui/components/MyCard.lui` → `ui_generated/components/my_card_ui.hpp`）

### 6.3 luic 工程模式

```
luic --project lvglpp.project.json
```

流程：

1. 读清单，加载全部 `.lui`
2. 建立依赖图（谁的 `children` 里出现了 `@Name`）
3. **拓扑排序**，按依赖顺序生成（被依赖者先）
4. **循环检测**：A 嵌 B、B 嵌 A → 报错并指出环路径
5. 校验：`kind` 与 `size` 的约束、`@Name` 是否存在、`exports` 里的内部 id 是否存在、参数键是否在 `exports` 中声明
6. （可选）`--emit-cmake <out.cmake>` 输出源文件与 include 目录列表

### 6.4 CMake 集成

用户侧保持简洁，最终仍走 CMake：

```cmake
include(${CMAKE_SOURCE_DIR}/ui_generated/lvglpp_project.cmake)
add_executable(my_app main.cpp ${LVGLPP_UI_SOURCES})
target_include_directories(my_app PRIVATE ${LVGLPP_UI_INCLUDE_DIRS})
```

`lvglpp_project.cmake` 由 luic 生成（或 CMake 侧直接 `file(GLOB ui/*.lui)` 再驱动 luic）。

> **待确认**：清单权威形式取"JSON 清单 + luic 生成 cmake 片段"（本文推荐）还是"清单本身即 CMake 片段，由 luic 解析"。推荐前者：JSON 更适合工具读写，CMake 片段是产物而非事实来源。

---

## 7. 设计器工程化

目标是**工程级设计器**，且**图形化是唯一编辑路径**（不做代码模式）。

### 7.1 工程视图

- "打开"变为**打开工程**（含清单的目录）；打开单个 `.lui` 时向上查找最近的清单
- 左侧新增"工程"视图，按 `kind` 分两组：**组件** / **屏幕**
- 设计器同一时刻只编辑其中一个文件，但始终知道整个工程

### 7.2 新建组件 / 新建屏幕

`文件 → 新建组件…` / `新建屏幕…`：

1. 输入名字 → **校验**：合法 C++ 标识符（`^[A-Za-z_][A-Za-z0-9_]*$`）、工程内不重名、不与内置组件重名
2. 选择存放位置（默认 `ui/components/` 或 `ui/screens/`）
3. 创建 `.lui`（写入 `kind`）→ 加入清单 → 自动打开

### 7.3 组件面板新增"项目组件"分组

列出工程内所有 `kind: component` 的文件，标签形如 `我的卡片(MyCard)`（沿用"中文名(类型ID)"的既有约定）。拖入画布即插入一个 `type: "@MyCard"` 的实例节点。

### 7.4 递归预览

设计器本来就直接从 `LuiDocument` 构建 LVGL 树（`LvglRenderer::buildTree`），因此实例的预览是**递归构建该子文件的 children 到实例根容器下**，不需要走代码生成。

需要处理：同一组件的多个实例必须各自独立建树；组件文件被修改后，引用它的文件需要重建预览。

### 7.5 选中实例时的属性面板

分两段：

- **实例自身**：位置、尺寸（作用在根容器上）
- **导出参数**：从子组件的 `exports` **动态生成**可编辑行（零额外维护 —— 这正是 `exports` 的第二个价值）

### 7.6 必须守住的不变量

> **只认识一部分字段，但绝不丢弃不认识的字段。**

`.lui` 是 JSON、在 git 里，用户仍可能手改（不提供专门模式，但挡不住）。解析 → 序列化必须保留未知键，否则用户手改后打开设计器再保存就是数据损坏。

---

## 8. 兼容性与迁移

- 现有 **38 个示例**均为 `kind` 缺省 → 按 `screen` 处理，**行为完全不变**
- `tools/lvgl-designer`：工程视图为新增，不打开工程时仍可按单文件模式工作（过渡期）
- luic：不带 `--project` 时行为与今天一致
- 文档同步：`json-ui-format.md` 需补充 `kind` / `exports` / `@Name` 三节

---

## 9. 分阶段落地

| 阶段 | 内容 | 交付判据 |
|---|---|---|
| **P1 组件化地基** | `kind` + 组件根容器 + `@Name` 实例 + `exports` + luic 生成 | 手写两个 `.lui`（一组件一屏幕），luic 生成可编译可运行的 C++；重复使用同一组件并设置不同参数 |
| **P2 工程清单** | 清单 + luic `--project`（依赖图／拓扑／循环检测／校验）+ CMake 集成 | 一个含多组件多屏幕的工程，一条命令编译，改组件后所有引用处生效 |
| **P3 设计器工程化** | 工程视图 + 新建组件/屏幕 + 项目组件面板 + 递归预览 + 参数编辑 | 全程图形化搭出 P2 的同一个工程 |
| **P4（预留）** | 组件间事件/回调 | 不在本轮 |

每阶段结束都要跑：现有 luic 测试（`tests/luic/run_tests.sh`）、设计器无头测试（`scripts/headless/run.sh`）、以及 38 个示例的回归（`examples/widgets_demo.cpp`）。

---

## 10. 预留：组件间事件/回调

本轮不做，但**要留口子**，否则后面返工。预留方向：

- 组件在 `exports` 之外可声明"可派发事件"，例如 `"emits": ["clicked"]`
- 父文件实例上声明处理方式，例如 `"on": { "clicked": "onCardClicked" }` → luic 生成 `m_card1->on_clicked(...)` 或在父类里生成虚函数/回调注册点
- 设计器侧需要"事件"编辑区（可后置）

**关键决定**：事件函数签名与生命周期归属（父对象持有回调 vs 组件自持）—— 待 P1 定稿时确认，避免与根容器的删除语义冲突。

---

## 11. 待确认决策（需评审拍板）

| # | 议题 | 我的推荐 | 理由 |
|---|---|---|---|
| 1 | 组件根容器默认尺寸 | **自适应内容**（`LV_SIZE_CONTENT`），实例上可覆盖 | 父级 flex/grid 里最自然；固定值会让父级布局处处要手调 |
| 2 | 工程清单权威形式 | **JSON 清单 + luic 生成 cmake 片段** | JSON 便于工具读写；CMake 片段是产物，避免两份事实来源 |
| 3 | `exports` 写法 | **`"caption": "m_title.text"` 简洁形式** | 可读性最好；更显式的 `{"target":..,"prop":..}` 冗长且无额外收益 |
| 4 | `int_pair` 类导出的 setter 形态 | `set_size(int w, int h)`（直接透传两个参数） | 直观，调用点可读 |
| 5 | 实例上未声明参数的处理 | **报错**（而非忽略） | 防止拼写错误静默失效 |

---

## 附录 A：一个最小完整工程

```
MyProject/
├── lvglpp.project.json
├── main.cpp
└── ui/
    ├── components/
    │   └── MyCard.lui
    └── screens/
        └── MainScreen.lui
```

`MyCard.lui`

```json
{
  "version": "1.0",
  "name": "MyCard",
  "kind": "component",
  "exports": { "caption": "m_title.text", "accent": "m_bar.bg_color" },
  "children": [
    { "id": "m_title", "type": "LLabel", "props": { "text": "默认标题", "pos": [8, 8] } },
    { "id": "m_bar",   "type": "LBar",   "props": { "value": 50, "pos": [8, 40], "size": [160, 12] } }
  ]
}
```

`MainScreen.lui`

```json
{
  "version": "1.0",
  "name": "MainScreen",
  "kind": "screen",
  "size": [400, 320],
  "children": [
    { "id": "card1", "type": "@MyCard",
      "props": { "pos": [10, 10], "size": [180, 120], "caption": "第一张", "accent": "0xFF5722" } },
    { "id": "card2", "type": "@MyCard",
      "props": { "pos": [200, 10], "size": [180, 120], "caption": "第二张" } }
  ]
}
```

`main.cpp`

```cpp
#include "lvgl++/lvgl_widgets.hpp"
#include "main_screen_ui.hpp"
using namespace lvgl;

int main() {
    lv_init();
    lv_display_t* disp = lv_sdl_window_create(MainScreen_UI::kScreenWidth,
                                             MainScreen_UI::kScreenHeight);
    (void)disp;
    MainScreen_UI ui;
    ui.setup(nullptr);
    while (true) { lv_timer_handler(); }
}
```

---

## 附录 B：与 QML / Qt 的取舍说明

**借鉴**：文件即组件类型、实例化语法、参数（`property`）模型。

**不借鉴**：

| QML 的做法 | 我们的做法 | 原因 |
|---|---|---|
| `Q_PROPERTY` / moc / 元对象反射 | 生成普通 C++ setter | 编译期检查，无运行时字符串查找，无引擎 |
| 运行时绑定引擎 / 响应式 | 静态代码生成 | LVGL 是纯 C 直接调用；静态生成零运行时开销 |
| `.ui`/`.qml` 与逻辑语言分离 | `.lui` 是纯声明式**数据** | 避免再发明一门语言 |
| 逐实例响应式绑定 | 默认不做，必要时 opt-in | QML 最重、最易失控的部分 |
