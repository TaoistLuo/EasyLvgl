# 构建系统配置完成

## 更新内容

### CMakeLists.txt 更新

#### 主要改动
1. ✅ 项目名称改为 `lvgl_cpp`
2. ✅ C++标准设置为 C++14
3. ✅ 添加 lvgl++ 头文件路径
4. ✅ 添加 3 个示例程序编译
5. ✅ 添加自定义运行目标

#### 新增编译目标

```cmake
# 示例程序
add_executable(layer1_demo examples/layer1_demo.cpp)
add_executable(layer2_demo examples/layer2_demo.cpp)
add_executable(complete_demo examples/complete_demo.cpp)

# 运行目标
make run              # 运行 main
make run_layer1       # 运行 layer1_demo
make run_layer2       # 运行 layer2_demo
make run_complete     # 运行 complete_demo
```

---

## 编译步骤

### 1. 配置构建
```bash
mkdir -p build
cd build
cmake ..
```

### 2. 编译所有目标
```bash
make -j$(nproc)
```

### 3. 编译单个目标
```bash
make main           # 主程序
make layer1_demo    # Layer 1 演示
make layer2_demo    # Layer 2 演示
make complete_demo  # 完整演示
```

---

## 运行程序

### 方式1：直接运行
```bash
cd bin
./main
./layer1_demo
./layer2_demo
./complete_demo
```

### 方式2：使用 make 目标
```bash
cd build
make run              # 运行 main
make run_layer1       # 运行 layer1_demo
make run_layer2       # 运行 layer2_demo
make run_complete     # 运行 complete_demo
```

---

## 编译配置

### C++ 标准
- **C++14** - 支持 lambda、auto、移动语义等现代特性

### 包含路径
```cmake
include_directories(${PROJECT_SOURCE_DIR})
```
这样可以使用：
```cpp
#include "lvgl++/lvgl_core.hpp"
#include "lvgl++/lvgl_widgets.hpp"
```

### 链接库
- lvgl - LVGL 核心库
- lvgl::demos - LVGL 演示
- SDL2 - 显示和输入
- pthread - 线程支持
- m - 数学库

---

## 目录结构

```
lv_port_linux/
├── CMakeLists.txt          # 主构建文件 ✅ 已更新
├── lvgl/                   # LVGL 源码
├── lvgl++/                 # LVGL++ 头文件
│   ├── core/
│   ├── widgets/
│   ├── containers/
│   └── dialogs/
├── examples/               # 示例程序
│   ├── layer1_demo.cpp
│   ├── layer2_demo.cpp
│   └── complete_demo.cpp
├── build/                  # 构建目录
└── bin/                    # 可执行文件输出
    ├── main
    ├── layer1_demo
    ├── layer2_demo
    └── complete_demo
```

---

## 编译输出

### 成功编译后
```
bin/
├── main              # 主程序（LVGL 原生 demo）
├── layer1_demo       # Layer 1 演示
├── layer2_demo       # Layer 2 演示
└── complete_demo     # 完整功能演示
```

---

## 常见问题

### 1. SDL2 未找到
```bash
# Ubuntu/Debian
sudo apt-get install libsdl2-dev libsdl2-image-dev

# Fedora
sudo dnf install SDL2-devel SDL2_image-devel
```

### 2. 编译错误
```bash
# 清理重新编译
rm -rf build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 3. 头文件找不到
确保 CMakeLists.txt 中有：
```cmake
include_directories(${PROJECT_SOURCE_DIR})
```

---

## 下一步

### 1. 编译测试 ⚠️ 立即执行
```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

### 2. 运行测试
```bash
cd bin
./layer1_demo
./layer2_demo
./complete_demo
```

### 3. 检查问题
- 编译警告
- 运行时错误
- 内存泄漏

### 4. 编写单元测试
- 使用 Google Test
- 测试每个组件
- 测试内存管理

---

**配置完成日期：** 2026-03-15  
**状态：** ✅ CMakeLists.txt 已更新  
**下一步：** 执行编译测试
