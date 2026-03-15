# LVGL++ Designer VSCode 插件指南

## 概述

LVGL++ Designer 是一个 VSCode 插件，提供可视化编辑 .lui 文件的能力。
拖拽组件到画布，编辑属性，保存后自动生成 C++ 代码。

## 环境要求

- Node.js 18+
- npm 9+
- VSCode 1.80+
- TypeScript 5+

## 编译插件

### 方式 1：开发模式（推荐）

```bash
cd vscode-lvgl-designer

# 安装依赖
npm install

# 编译 TypeScript
npm run compile

# 在 VSCode 中按 F5 启动 Extension Development Host
# 这会打开一个新的 VSCode 窗口，插件已加载
```

### 方式 2：打包为 .vsix 安装包

```bash
cd vscode-lvgl-designer

# 安装依赖
npm install

# 安装打包工具
npm install -g @vscode/vsce

# 打包
vsce package

# 生成 lvgl-designer-0.1.0.vsix
```

## 安装插件

### 方式 1：开发模式安装

1. 用 VSCode 打开 `vscode-lvgl-designer` 目录
2. 按 `F5` 启动 Extension Development Host
3. 在新窗口中打开你的 LVGL++ 项目
4. 打开任意 `.lui` 文件，自动启动可视化编辑器

### 方式 2：从 .vsix 文件安装

```bash
# 命令行安装
code --install-extension lvgl-designer-0.1.0.vsix
```

或者在 VSCode 中：
1. 打开命令面板（Ctrl+Shift+P）
2. 输入 "Install from VSIX"
3. 选择 `lvgl-designer-0.1.0.vsix` 文件

### 方式 3：复制到扩展目录

```bash
# Linux
cp -r vscode-lvgl-designer ~/.vscode/extensions/lvgl-designer

# macOS
cp -r vscode-lvgl-designer ~/Library/Application\ Support/Code/User/extensions/lvgl-designer

# Windows
xcopy vscode-lvgl-designer %USERPROFILE%\.vscode\extensions\lvgl-designer /E /I
```

## 使用方法

### 1. 创建新界面

- 命令面板（Ctrl+Shift+P）→ "LVGL++: New Screen"
- 输入屏幕名称（如 MainScreen）
- 自动创建 `ui/mainscreen.lui` 并打开编辑器

### 2. 编辑界面

打开任意 `.lui` 文件，VSCode 自动启动可视化编辑器：

```
┌──────────┬──────────────────────┬───────────────────┐
│ 组件面板 │      画布区域        │    属性面板       │
│          │                      │                   │
│ [Button] │  拖拽组件到这里      │ ID: btn_login     │
│ [Label]  │  点击选中组件        │ Text: Login       │
│ [Slider] │  拖拽移动组件        │ Size: 120 x 50    │
│ ...      │                      │ Events: ☑ clicked │
└──────────┴──────────────────────┴───────────────────┘
│ 代码预览（实时生成的 C++ 代码）                      │
└──────────────────────────────────────────────────────┘
```

### 3. 操作说明

| 操作 | 方法 |
|------|------|
| 添加组件 | 从左侧面板拖拽到画布 |
| 选中组件 | 点击画布上的组件 |
| 移动组件 | 拖拽已选中的组件 |
| 编辑属性 | 在右侧属性面板修改 |
| 勾选事件 | 在属性面板底部勾选 |
| 删除组件 | 选中后点击 🗑️ Delete |
| 保存 | 点击 💾 Save 或 Ctrl+S |
| 编译 | 点击 ⚡ Compile |

### 4. 编译生成 C++ 代码

保存 .lui 文件后：

```bash
cd build
make ui_compile    # 调用 luic 生成 C++ 代码
make luic_demo     # 编译并链接
```

或者在 VSCode 中点击 ⚡ Compile 按钮。

### 5. 编写业务代码

```cpp
#include "ui_generated/main_screen_ui.hpp"

class MainScreen : public lvgl::MainScreen_UI {
public:
    void on_btn_login_clicked(lv_event_t* e) override {
        printf("Login!\n");
    }
};
```

## 支持的组件

| 组件 | 图标 | 说明 |
|------|------|------|
| LLabel | 📝 | 文本标签 |
| LButton | 🔘 | 按钮 |
| LSlider | 🎚️ | 滑块 |
| LBar | 📊 | 进度条 |
| LCheckbox | ☑️ | 复选框 |
| LSwitch | 🔀 | 开关 |
| LDropdown | 📋 | 下拉列表 |
| LTextArea | 📄 | 文本输入 |
| LArc | ⭕ | 圆弧进度 |
| LSpinner | 🔄 | 加载动画 |
| LLed | 💡 | LED 指示灯 |
| LVStack | 📦 | 垂直布局 |
| LHStack | 📦 | 水平布局 |
| LTable | 📋 | 表格 |
| LChart | 📈 | 图表 |
| LImage | 🖼️ | 图片 |

## 故障排除

### 插件未加载
- 确认 VSCode 版本 >= 1.80
- 确认 `npm run compile` 无错误
- 查看 VSCode 输出面板（Output → LVGL++ Designer）

### .lui 文件未打开编辑器
- 确认文件扩展名是 `.lui`
- 右键文件 → "Open With..." → 选择 "LVGL++ Designer"

### 编译失败
- 确认 `bin/luic` 存在且可执行
- 运行 `cd build && make luic` 重新编译 luic
- 检查 .lui 文件 JSON 格式是否正确

## 项目结构

```
vscode-lvgl-designer/
├── package.json              # 插件配置
├── tsconfig.json             # TypeScript 配置
├── README.md                 # 说明文档
└── src/
    ├── extension.ts          # 插件入口
    └── LuiEditorProvider.ts  # 可视化编辑器
```
