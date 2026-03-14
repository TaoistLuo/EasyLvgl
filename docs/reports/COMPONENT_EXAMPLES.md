# LVGL++ 组件使用示例

所有 31 个组件的完整使用示例。

---

## 核心组件（8个）

### 1. LButton - 按钮
```cpp
LButton btn;
btn.text("Click Me")
   .size(120, 50)
   .primary()
   .on_click([](lv_event_t*) {
       printf("Button clicked!\n");
   });
```

### 2. LLabel - 标签
```cpp
LLabel label;
label.text("Hello LVGL++")
     .color(0x2196F3)
     .align(LV_ALIGN_CENTER);
```

### 3. LSlider - 滑块
```cpp
LSlider slider;
slider.range(0, 100)
      .value(50)
      .size(200, 10)
      .on_change([](lv_event_t* e) {
          auto* slider = (lv_obj_t*)lv_event_get_target(e);
          int val = lv_slider_get_value(slider);
          printf("Value: %d\n", val);
      });
```

### 4. LCheckbox - 复选框
```cpp
LCheckbox cb;
cb.text("Enable Feature")
  .checked(true)
  .on_change([](lv_event_t* e) {
      auto* cb = (lv_obj_t*)lv_event_get_target(e);
      bool checked = lv_obj_has_state(cb, LV_STATE_CHECKED);
      printf("Checked: %d\n", checked);
  });
```

### 5. LSwitch - 开关
```cpp
LSwitch sw;
sw.on()
  .on_change([](lv_event_t* e) {
      auto* sw = (lv_obj_t*)lv_event_get_target(e);
      bool on = lv_obj_has_state(sw, LV_STATE_CHECKED);
      printf("Switch: %s\n", on ? "ON" : "OFF");
  });
```

### 6. LDropdown - 下拉框
```cpp
LDropdown dd;
dd.options("Option 1\nOption 2\nOption 3")
  .selected(0)
  .on_change([](lv_event_t* e) {
      auto* dd = (lv_obj_t*)lv_event_get_target(e);
      uint16_t idx = lv_dropdown_get_selected(dd);
      printf("Selected: %d\n", idx);
  });
```

### 7. LTextArea - 文本输入
```cpp
LTextArea ta;
ta.text("Type here...")
  .size(200, 80)
  .placeholder("Enter text")
  .one_line(false)
  .on_change([](lv_event_t*) {
      printf("Text changed\n");
  });
```

### 8. LImage - 图片
```cpp
LImage img;
img.src("path/to/image.png")
   .size(100, 100)
   .align(LV_ALIGN_CENTER);
```

---

## 高频组件（6个）

### 9. LArc - 圆弧进度
```cpp
LArc arc;
arc.range(0, 100)
   .value(75)
   .rotation(270)
   .bg_angles(0, 360)
   .colors(0xCCCCCC, 0x2196F3)
   .size(150)
   .on_change([](lv_event_t* e) {
       auto* arc = (lv_obj_t*)lv_event_get_target(e);
       int val = lv_arc_get_value(arc);
       printf("Arc value: %d\n", val);
   });
```

### 10. LBar - 进度条
```cpp
LBar bar;
bar.range(0, 100)
   .value(60, LV_ANIM_ON)
   .colors(0xEEEEEE, 0x4CAF50)
   .size(200, 20);

bar.value(80, LV_ANIM_ON);
```

### 11. LSpinner - 加载动画
```cpp
LSpinner spinner;
spinner.size(50)
       .color(0x2196F3)
       .align(LV_ALIGN_CENTER);
```

### 12. LLed - LED指示灯
```cpp
LLed led_green;
led_green.color(0x00FF00)
         .brightness(255)
         .on()
         .size(30);

LLed led_red;
led_red.color(0xFF0000)
       .brightness(200)
       .off()
       .size(30);

led_green.toggle();
```

### 13. LLine - 线条
```cpp
static lv_point_precise_t points[] = {
    {0, 0}, {50, 25}, {100, 0}, {150, 25}
};

LLine line;
line.points(points, 4)
    .width(3)
    .color(0xFF0000);
```

### 14. LList - 列表
```cpp
LList list;
list.size(200, 300);

list.add_text("Section 1");
list.add_button(LV_SYMBOL_FILE, "Item 1");
list.add_button(LV_SYMBOL_DIRECTORY, "Item 2");

list.add_text("Section 2");
list.add_button(LV_SYMBOL_OK, "Item 3");
```

---

## 中频组件（10个）

### 15. LTable - 表格
```cpp
LTable table;
table.row_count(3)
     .column_count(3)
     .size(300, 200);

table.cell_value(0, 0, "Name");
table.cell_value(0, 1, "Age");
table.cell_value(0, 2, "City");

table.cell_value(1, 0, "Alice");
table.cell_value(1, 1, "25");
table.cell_value(1, 2, "Beijing");

table.column_width(0, 100);
table.column_width(1, 60);
table.column_width(2, 120);
```

### 16. LTabView - 标签页
```cpp
LTabView tabview;
tabview.size(400, 300);

auto tab1 = tabview.add_tab("Tab 1");
auto tab2 = tabview.add_tab("Tab 2");
auto tab3 = tabview.add_tab("Tab 3");

LLabel label1;
label1.text("Content of Tab 1");
lv_obj_set_parent(label1.native(), tab1);

tabview.set_active(0, LV_ANIM_ON);
```

### 17. LMenu - 菜单
```cpp
LMenu menu;
menu.size(300, 400);

auto page = menu.page_create("Settings");
auto section = menu.section_create(page);
auto cont = menu.cont_create(section);

menu.set_page(page);
```

### 18. LRoller - 滚轮选择器
```cpp
LRoller roller;
roller.options("January\nFebruary\nMarch\nApril\nMay\nJune")
      .selected(2)
      .visible_row_count(3)
      .size(120, 150)
      .on_change([](lv_event_t* e) {
          auto* roller = (lv_obj_t*)lv_event_get_target(e);
          uint16_t idx = lv_roller_get_selected(roller);
          printf("Month: %d\n", idx + 1);
      });
```

### 19. LScale - 刻度尺
```cpp
LScale scale;
scale.mode(LV_SCALE_MODE_HORIZONTAL_BOTTOM)
     .range(0, 100)
     .total_tick_count(11)
     .major_tick_every(5)
     .size(300, 60);
```

### 20. LSpan - 富文本
```cpp
LSpan span;
span.size(300, 100)
    .align(LV_TEXT_ALIGN_LEFT)
    .mode(LV_SPAN_MODE_BREAK);

auto s1 = span.new_span();
lv_span_set_text(s1, "This is ");

auto s2 = span.new_span();
lv_span_set_text(s2, "rich text ");
lv_style_set_text_color(&s2->style, lv_color_hex(0xFF0000));

auto s3 = span.new_span();
lv_span_set_text(s3, "example!");
```

### 21. LSpinbox - 数字输入框
```cpp
LSpinbox spinbox;
spinbox.range(0, 100)
       .value(50)
       .step(5)
       .digit_format(3, 0)
       .size(120, 50);

LButton btn_inc;
btn_inc.text("+")
       .size(50, 50)
       .on_click([&spinbox](lv_event_t*) {
           spinbox.increment();
       });

LButton btn_dec;
btn_dec.text("-")
       .size(50, 50)
       .on_click([&spinbox](lv_event_t*) {
           spinbox.decrement();
       });

int current = spinbox.value();
printf("Current value: %d\n", current);
```

### 22. LButtonMatrix - 按钮矩阵
```cpp
static const char* btnm_map[] = {
    "1", "2", "3", "\n",
    "4", "5", "6", "\n",
    "7", "8", "9", "\n",
    "0", LV_SYMBOL_BACKSPACE, ""
};

LButtonMatrix btnm;
btnm.map(btnm_map)
    .size(200, 200)
    .on_click([](lv_event_t* e) {
        auto* btnm = (lv_obj_t*)lv_event_get_target(e);
        uint16_t idx = lv_buttonmatrix_get_selected_button(btnm);
        printf("Button: %d\n", idx);
    });
```

### 23. LImageButton - 图片按钮
```cpp
LImageButton imgbtn;
imgbtn.src(LV_IMAGEBUTTON_STATE_RELEASED, "path/to/btn_released.png")
      .src(LV_IMAGEBUTTON_STATE_PRESSED, "path/to/btn_pressed.png")
      .size(100, 100)
      .on_click([](lv_event_t*) {
          printf("Image button clicked!\n");
      });
```

### 24. LMsgBox - 消息框
```cpp
LButton show_btn;
show_btn.text("Show Message")
        .on_click([](lv_event_t*) {
            LMsgBox msgbox(lv_scr_act());
            msgbox.title("Information")
                  .text("This is a message box!")
                  .add_close_button();
            
            auto ok_btn = msgbox.add_footer_button("OK");
            auto cancel_btn = msgbox.add_footer_button("Cancel");
        });
```

---

## 低频组件（7个）

### 25. LCalendar - 日历
```cpp
LCalendar calendar;
calendar.today_date(2026, 3, 15)
        .showed_date(2026, 3)
        .size(300, 300);

auto today = calendar.get_today_date();
printf("Today: %d-%d-%d\n", today->year, today->month, today->day);
```

### 26. LChart - 图表
```cpp
LChart chart;
chart.type(LV_CHART_TYPE_LINE)
     .point_count(10)
     .range(LV_CHART_AXIS_PRIMARY_Y, 0, 100)
     .size(300, 200);

auto series1 = chart.add_series(0xFF0000, LV_CHART_AXIS_PRIMARY_Y);
auto series2 = chart.add_series(0x0000FF, LV_CHART_AXIS_PRIMARY_Y);

for(int i = 0; i < 10; i++) {
    chart.set_next_value(series1, 20 + i * 5);
    chart.set_next_value(series2, 80 - i * 5);
}
```

### 27. LCanvas - 画布
```cpp
static uint8_t cbuf[LV_CANVAS_BUF_SIZE(200, 150, 32, LV_STRIDE_AUTO)];

LCanvas canvas;
canvas.buffer(cbuf, 200, 150, LV_COLOR_FORMAT_ARGB8888)
      .size(200, 150);

canvas.fill_bg(0xFFFFFF, LV_OPA_COVER);

for(int i = 0; i < 200; i += 10) {
    canvas.px(i, i * 150 / 200, 0xFF0000, LV_OPA_COVER);
}
```

### 28. LKeyboard - 键盘
```cpp
LTextArea ta;
ta.size(400, 100)
  .placeholder("Type with keyboard...");

LKeyboard keyboard;
keyboard.textarea(ta.native())
        .mode(LV_KEYBOARD_MODE_TEXT_LOWER)
        .size(400, 200);
```

### 29. LAnimImage - 动画图片
```cpp
static const void* anim_imgs[] = {
    &img_frame1,
    &img_frame2,
    &img_frame3,
    NULL
};

LAnimImage animimg;
animimg.src(anim_imgs, 3)
       .duration(1000)
       .repeat_count(LV_ANIM_REPEAT_INFINITE)
       .size(100, 100)
       .start();
```

### 30. LTileView - 平铺视图
```cpp
LTileView tileview;
tileview.size(400, 300);

auto tile1 = tileview.add_tile(0, 0, LV_DIR_RIGHT);
auto tile2 = tileview.add_tile(1, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
auto tile3 = tileview.add_tile(2, 0, LV_DIR_LEFT);

LLabel label1;
label1.text("Tile 1");
lv_obj_set_parent(label1.native(), tile1);
lv_obj_center(label1.native());

tileview.set_tile(tile1, LV_ANIM_OFF);
```

### 31. LWin - 窗口
```cpp
LWin win;
win.size(400, 300);

win.add_title("Window Title");
auto close_btn = win.add_button(LV_SYMBOL_CLOSE, 50);

auto header = win.get_header();
auto content = win.get_content();

LLabel label;
label.text("Window content area");
lv_obj_set_parent(label.native(), content);
lv_obj_center(label.native());
```

---

## 容器组件（2个）

### 32. LVStack - 垂直布局
```cpp
LVStack vstack;
vstack.size(200, 300)
      .padding(10)
      .gap(5);

LButton btn1;
btn1.text("Button 1").size(180, 40);
vstack.add(btn1);

LButton btn2;
btn2.text("Button 2").size(180, 40);
vstack.add(btn2);

LButton btn3;
btn3.text("Button 3").size(180, 40);
vstack.add(btn3);
```

### 33. LHStack - 水平布局
```cpp
LHStack hstack;
hstack.size(300, 60)
      .padding(10)
      .gap(5);

LButton btn1;
btn1.text("1").size(80, 40);
hstack.add(btn1);

LButton btn2;
btn2.text("2").size(80, 40);
hstack.add(btn2);

LButton btn3;
btn3.text("3").size(80, 40);
hstack.add(btn3);
```

---

## 完整示例程序

```cpp
#include "lvgl++/lvgl_widgets.hpp"
#include "lvgl/lvgl.h"

using namespace lvgl;

int main() {
    lv_init();
    lv_linux_disp_init();
    lv_sdl_mouse_create();

    LVStack main_container;
    main_container.size(400, 600).padding(20).gap(10);

    LLabel title;
    title.text("LVGL++ Demo").color(0x2196F3);
    main_container.add(title);

    LButton btn;
    btn.text("Click Me").size(360, 50).primary();
    btn.on_click([](lv_event_t*) {
        printf("Button clicked!\n");
    });
    main_container.add(btn);

    LSlider slider;
    slider.range(0, 100).value(50).size(360, 10);
    main_container.add(slider);

    LBar bar;
    bar.range(0, 100).value(75).size(360, 20);
    main_container.add(bar);

    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}
```

---

## 编译和运行

```bash
cd build
cmake ..
make all_components_demo
./bin/all_components_demo
```

---

**所有 31 个组件示例完成！** 🎉
