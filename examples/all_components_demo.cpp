/**
 * @file all_components_demo.cpp
 * @brief LVGL++ All Components Demo - 31 Components Usage Examples
 * @version 2.0.0
 * @date 2026-03-15
 */

#include "lvgl++/lvgl_widgets.hpp"
#include "lvgl/lvgl.h"
#include <cstdio>

using namespace lvgl;

int main(void) {
    lv_init();
    lv_linux_disp_init();
    lv_sdl_mouse_create();

    printf("=== LVGL++ All Components Demo ===\n\n");

    // ========================================
    // 核心组件示例（8个）
    // ========================================
    
    printf("1. LButton - 按钮\n");
    {
        LButton btn;
        btn.text("Click Me")
           .size(120, 50)
           .pos(10, 10)
           .primary()
           .on_click([](lv_event_t*) {
               printf("   Button clicked!\n");
           });
    }

    printf("2. LLabel - 标签\n");
    {
        LLabel label;
        label.text("Hello LVGL++")
             .pos(150, 20)
             .color(0x2196F3);
    }

    printf("3. LSlider - 滑块\n");
    {
        LSlider slider;
        slider.range(0, 100)
              .value(50)
              .size(200, 10)
              .pos(10, 80)
              .on_change([](lv_event_t* e) {
                  auto* slider = (lv_obj_t*)lv_event_get_target(e);
                  int val = lv_slider_get_value(slider);
                  printf("   Slider value: %d\n", val);
              });
    }

    printf("4. LCheckbox - 复选框\n");
    {
        LCheckbox cb;
        cb.text("Enable Feature")
          .pos(10, 110)
          .checked(true)
          .on_change([](lv_event_t* e) {
              auto* cb = (lv_obj_t*)lv_event_get_target(e);
              bool checked = lv_obj_has_state(cb, LV_STATE_CHECKED);
              printf("   Checkbox: %s\n", checked ? "checked" : "unchecked");
          });
    }

    printf("5. LSwitch - 开关\n");
    {
        LSwitch sw;
        sw.pos(10, 150)
          .on()
          .on_change([](lv_event_t* e) {
              auto* sw = (lv_obj_t*)lv_event_get_target(e);
              bool on = lv_obj_has_state(sw, LV_STATE_CHECKED);
              printf("   Switch: %s\n", on ? "ON" : "OFF");
          });
    }

    printf("6. LDropdown - 下拉框\n");
    {
        LDropdown dd;
        dd.options("Option 1\nOption 2\nOption 3")
          .pos(10, 200)
          .selected(0)
          .on_change([](lv_event_t* e) {
              auto* dd = (lv_obj_t*)lv_event_get_target(e);
              uint16_t idx = lv_dropdown_get_selected(dd);
              printf("   Dropdown selected: %d\n", idx);
          });
    }

    printf("7. LTextArea - 文本输入\n");
    {
        LTextArea ta;
        ta.text("Type here...")
          .pos(10, 250)
          .size(200, 80)
          .placeholder("Enter text")
          .on_change([](lv_event_t*) {
              printf("   Text changed\n");
          });
    }

    printf("8. LImage - 图片\n");
    {
        LImage img;
        img.pos(250, 10)
           .size(100, 100);
    }

    // ========================================
    // 高频组件示例（6个）
    // ========================================

    printf("\n9. LArc - 圆弧进度\n");
    {
        LArc arc;
        arc.range(0, 100)
           .value(75)
           .rotation(270)
           .bg_angles(0, 360)
           .colors(0xCCCCCC, 0x2196F3)
           .size(100)
           .pos(400, 10);
    }

    printf("10. LBar - 进度条\n");
    {
        LBar bar;
        bar.range(0, 100)
           .value(60, LV_ANIM_ON)
           .colors(0xEEEEEE, 0x4CAF50)
           .size(200, 20)
           .pos(400, 130);
    }

    printf("11. LSpinner - 加载动画\n");
    {
        LSpinner spinner;
        spinner.size(50)
               .color(0x2196F3)
               .pos(400, 170);
    }

    printf("12. LLed - LED指示灯\n");
    {
        LLed led_green;
        led_green.color(0x00FF00)
                 .brightness(255)
                 .on()
                 .size(30)
                 .pos(400, 240);

        LLed led_red;
        led_red.color(0xFF0000)
               .brightness(200)
               .off()
               .size(30)
               .pos(450, 240);
    }

    printf("13. LLine - 线条\n");
    {
        static lv_point_precise_t points[] = {
            {0, 0}, {50, 25}, {100, 0}, {150, 25}
        };
        LLine line;
        line.points(points, 4)
            .width(3)
            .color(0xFF0000)
            .pos(400, 290);
    }

    printf("14. LList - 列表\n");
    {
        LList list;
        list.pos(10, 350)
            .size(200, 150);
        list.add_text("Section 1");
        list.add_button(LV_SYMBOL_FILE, "Item 1");
        list.add_button(LV_SYMBOL_DIRECTORY, "Item 2");
        list.add_text("Section 2");
        list.add_button(LV_SYMBOL_OK, "Item 3");
    }

    // ========================================
    // 中频组件示例（10个）
    // ========================================

    printf("\n15. LTable - 表格\n");
    {
        LTable table;
        table.row_count(3)
             .column_count(3)
             .pos(230, 350)
             .size(250, 150);
        
        table.cell_value(0, 0, "Name");
        table.cell_value(0, 1, "Age");
        table.cell_value(0, 2, "City");
        
        table.cell_value(1, 0, "Alice");
        table.cell_value(1, 1, "25");
        table.cell_value(1, 2, "Beijing");
        
        table.cell_value(2, 0, "Bob");
        table.cell_value(2, 1, "30");
        table.cell_value(2, 2, "Shanghai");
        
        table.column_width(0, 80);
        table.column_width(1, 60);
        table.column_width(2, 100);
    }

    printf("16. LTabView - 标签页\n");
    {
        LTabView tabview;
        tabview.pos(500, 350)
               .size(280, 200);
        
        auto tab1 = tabview.add_tab("Tab 1");
        auto tab2 = tabview.add_tab("Tab 2");
        auto tab3 = tabview.add_tab("Tab 3");
        
        LLabel label1;
        label1.text("Content of Tab 1");
        lv_obj_set_parent(label1.native(), tab1);
        
        LLabel label2;
        label2.text("Content of Tab 2");
        lv_obj_set_parent(label2.native(), tab2);
    }

    printf("17. LMenu - 菜单\n");
    {
        LMenu menu;
        menu.pos(10, 520)
            .size(200, 150);
        
        auto page = menu.page_create("Settings");
        menu.set_page(page);
    }

    printf("18. LRoller - 滚轮选择器\n");
    {
        LRoller roller;
        roller.options("January\nFebruary\nMarch\nApril\nMay\nJune")
              .selected(0)
              .visible_row_count(3)
              .pos(230, 520)
              .size(100, 150)
              .on_change([](lv_event_t* e) {
                  auto* roller = (lv_obj_t*)lv_event_get_target(e);
                  uint16_t idx = lv_roller_get_selected(roller);
                  printf("   Roller selected: %d\n", idx);
              });
    }

    printf("19. LScale - 刻度尺\n");
    {
        LScale scale;
        scale.mode(LV_SCALE_MODE_HORIZONTAL_BOTTOM)
             .range(0, 100)
             .total_tick_count(11)
             .major_tick_every(5)
             .pos(350, 520)
             .size(200, 50);
    }

    printf("20. LSpan - 富文本\n");
    {
        LSpan span;
        span.pos(350, 590)
            .size(200, 80)
            .align(LV_TEXT_ALIGN_LEFT)
            .mode(LV_SPAN_MODE_BREAK);
        
        auto s1 = span.new_span();
        lv_span_set_text(s1, "This is ");
        
        auto s2 = span.new_span();
        lv_span_set_text(s2, "rich text ");
        lv_style_set_text_color(&s2->style, lv_color_hex(0xFF0000));
        
        auto s3 = span.new_span();
        lv_span_set_text(s3, "example!");
    }

    printf("21. LSpinbox - 数字输入框\n");
    {
        LSpinbox spinbox;
        spinbox.range(0, 100)
               .value(50)
               .step(5)
               .digit_format(3, 0)
               .pos(570, 520)
               .size(100, 40);
        
        LButton btn_inc;
        btn_inc.text("+")
               .size(40, 40)
               .pos(680, 520)
               .on_click([&spinbox](lv_event_t*) {
                   spinbox.increment();
               });
        
        LButton btn_dec;
        btn_dec.text("-")
               .size(40, 40)
               .pos(570, 570)
               .on_click([&spinbox](lv_event_t*) {
                   spinbox.decrement();
               });
    }

    printf("22. LButtonMatrix - 按钮矩阵\n");
    {
        static const char* btnm_map[] = {"1", "2", "3", "\n",
                                         "4", "5", "6", "\n",
                                         "7", "8", "9", "\n",
                                         "0", LV_SYMBOL_BACKSPACE, ""};
        LButtonMatrix btnm;
        btnm.map(btnm_map)
            .pos(10, 690)
            .size(150, 150)
            .on_click([](lv_event_t* e) {
                auto* btnm = (lv_obj_t*)lv_event_get_target(e);
                uint16_t idx = lv_buttonmatrix_get_selected_button(btnm);
                printf("   Button matrix clicked: %d\n", idx);
            });
    }

    printf("23. LImageButton - 图片按钮\n");
    {
        LImageButton imgbtn;
        imgbtn.pos(180, 690)
              .size(80, 80)
              .on_click([](lv_event_t*) {
                  printf("   Image button clicked!\n");
              });
    }

    printf("24. LMsgBox - 消息框\n");
    {
        LButton show_msgbox;
        show_msgbox.text("Show MsgBox")
                   .pos(280, 690)
                   .size(120, 50)
                   .on_click([](lv_event_t*) {
                       LMsgBox msgbox(lv_scr_act());
                       msgbox.title("Information")
                             .text("This is a message box!")
                             .add_close_button();
                       auto btn = msgbox.add_footer_button("OK");
                       printf("   Message box shown\n");
                   });
    }

    // ========================================
    // 低频组件示例（7个）
    // ========================================

    printf("\n25. LCalendar - 日历\n");
    {
        LCalendar calendar;
        calendar.today_date(2026, 3, 15)
                .showed_date(2026, 3)
                .pos(420, 690)
                .size(250, 250);
    }

    printf("26. LChart - 图表\n");
    {
        LChart chart;
        chart.type(LV_CHART_TYPE_LINE)
             .point_count(10)
             .range(LV_CHART_AXIS_PRIMARY_Y, 0, 100)
             .pos(690, 690)
             .size(200, 150);
        
        auto series1 = chart.add_series(0xFF0000, LV_CHART_AXIS_PRIMARY_Y);
        auto series2 = chart.add_series(0x0000FF, LV_CHART_AXIS_PRIMARY_Y);
        
        for(int i = 0; i < 10; i++) {
            chart.set_next_value(series1, 20 + i * 5);
            chart.set_next_value(series2, 80 - i * 5);
        }
    }

    printf("27. LCanvas - 画布\n");
    {
        static uint8_t cbuf[LV_CANVAS_BUF_SIZE(100, 100, 32, LV_STRIDE_AUTO)];
        LCanvas canvas;
        canvas.buffer(cbuf, 100, 100, LV_COLOR_FORMAT_ARGB8888)
              .pos(910, 690)
              .size(100, 100);
        
        canvas.fill_bg(0xFFFFFF, LV_OPA_COVER);
        
        for(int i = 0; i < 100; i += 10) {
            canvas.px(i, i, 0xFF0000, LV_OPA_COVER);
            canvas.px(i, 99-i, 0x0000FF, LV_OPA_COVER);
        }
    }

    printf("28. LKeyboard - 键盘\n");
    {
        LTextArea ta_kb;
        ta_kb.pos(10, 960)
             .size(400, 80)
             .placeholder("Type with keyboard...");
        
        LKeyboard keyboard;
        keyboard.textarea(ta_kb.native())
                .pos(10, 1050)
                .size(400, 150);
    }

    printf("29. LAnimImage - 动画图片\n");
    {
        LAnimImage animimg;
        animimg.duration(1000)
               .repeat_count(LV_ANIM_REPEAT_INFINITE)
               .pos(430, 960)
               .size(100, 100);
    }

    printf("30. LTileView - 平铺视图\n");
    {
        LTileView tileview;
        tileview.pos(550, 960)
                .size(300, 200);
        
        auto tile1 = tileview.add_tile(0, 0, LV_DIR_RIGHT);
        auto tile2 = tileview.add_tile(1, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
        auto tile3 = tileview.add_tile(2, 0, LV_DIR_LEFT);
        
        LLabel label_t1;
        label_t1.text("Tile 1");
        lv_obj_set_parent(label_t1.native(), tile1);
        lv_obj_center(label_t1.native());
        
        LLabel label_t2;
        label_t2.text("Tile 2");
        lv_obj_set_parent(label_t2.native(), tile2);
        lv_obj_center(label_t2.native());
        
        LLabel label_t3;
        label_t3.text("Tile 3");
        lv_obj_set_parent(label_t3.native(), tile3);
        lv_obj_center(label_t3.native());
    }

    printf("31. LWin - 窗口\n");
    {
        LWin win;
        win.pos(870, 960)
           .size(300, 200);
        
        win.add_title("Window Title");
        auto close_btn = win.add_button(LV_SYMBOL_CLOSE, 40);
        
        auto content = win.get_content();
        LLabel label_win;
        label_win.text("Window content area");
        lv_obj_set_parent(label_win.native(), content);
    }

    printf("\n=== All 31 components demonstrated! ===\n");
    printf("Press Ctrl+C to exit\n\n");

    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}
