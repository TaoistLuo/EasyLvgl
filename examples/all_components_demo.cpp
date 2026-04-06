#include "lvgl++/lvgl_widgets.hpp"
#include "lvgl/lvgl.h"
#include <cstdio>
#include <unistd.h>

using namespace lvgl;

int main(void) {
    lv_init();
    lv_linux_disp_init();
    lv_sdl_mouse_create();

    printf("=== LVGL++ All Components Demo ===\n\n");

    printf("1. LButton\n");
    {
        LButton btn;
        btn.set_text("Click Me")
           .set_size(120, 50)
           .set_pos(10, 10)
           .primary()
           .on_click([](lv_event_t*) {
               printf("   Button clicked!\n");
           });
    }

    printf("2. LLabel\n");
    {
        LLabel label;
        label.set_text("Hello LVGL++")
             .set_pos(150, 20)
             .set_color(0x2196F3);
    }

    printf("3. LSlider\n");
    {
        LSlider slider;
        slider.set_range(0, 100)
              .set_value(50)
              .set_size(200, 10)
              .set_pos(10, 80)
              .on_change([](lv_event_t* e) {
                  auto* slider = (lv_obj_t*)lv_event_get_target(e);
                  int val = lv_slider_get_value(slider);
                  printf("   Slider value: %d\n", val);
              });
    }

    printf("4. LCheckbox\n");
    {
        LCheckbox cb;
        cb.set_text("Enable Feature")
          .set_pos(10, 110)
          .set_checked(true)
          .on_change([](lv_event_t* e) {
              auto* cb = (lv_obj_t*)lv_event_get_target(e);
              bool checked = lv_obj_has_state(cb, LV_STATE_CHECKED);
              printf("   Checkbox: %s\n", checked ? "checked" : "unchecked");
          });
    }

    printf("5. LSwitch\n");
    {
        LSwitch sw;
        sw.set_pos(10, 150)
          .on()
          .on_change([](lv_event_t* e) {
              auto* sw = (lv_obj_t*)lv_event_get_target(e);
              bool on = lv_obj_has_state(sw, LV_STATE_CHECKED);
              printf("   Switch: %s\n", on ? "ON" : "OFF");
          });
    }

    printf("6. LComboBox\n");
    {
        LComboBox dd;
        dd.set_options("Option 1\nOption 2\nOption 3")
          .set_pos(10, 200)
          .set_selected(0)
          .on_change([](lv_event_t* e) {
              auto* dd = (lv_obj_t*)lv_event_get_target(e);
              uint16_t idx = lv_dropdown_get_selected(dd);
              printf("   Dropdown selected: %d\n", idx);
          });
    }

    printf("7. LLineEdit\n");
    {
        LLineEdit ta;
        ta.set_text("Type here...")
          .set_pos(10, 250)
          .set_size(200, 80)
          .set_placeholder("Enter text")
          .on_change([](lv_event_t*) {
              printf("   Text changed\n");
          });
    }

    printf("8. LImage\n");
    {
        LImage img;
        img.set_pos(250, 10)
           .set_size(100, 100);
    }

    printf("\n9. LArc\n");
    {
        LArc arc;
        arc.set_range(0, 100)
           .set_value(75)
           .set_rotation(270)
           .set_bg_angles(0, 360)
           .set_colors(0xCCCCCC, 0x2196F3)
           .set_size(100)
           .set_pos(400, 10);
    }

    printf("10. LBar\n");
    {
        LBar bar;
        bar.set_range(0, 100)
           .set_value(60, LV_ANIM_ON)
           .set_colors(0xEEEEEE, 0x4CAF50)
           .set_size(200, 20)
           .set_pos(400, 130);
    }

    printf("11. LSpinner\n");
    {
        LSpinner spinner;
        spinner.set_size(50)
               .set_color(0x2196F3)
               .set_pos(400, 170);
    }

    printf("12. LLed\n");
    {
        LLed led_green;
        led_green.set_color(0x00FF00)
                 .set_brightness(255)
                 .on()
                 .set_size(30)
                 .set_pos(400, 240);

        LLed led_red;
        led_red.set_color(0xFF0000)
               .set_brightness(200)
               .off()
               .set_size(30)
               .set_pos(450, 240);
    }

    printf("13. LLine\n");
    {
        static lv_point_precise_t points[] = {
            {0, 0}, {50, 25}, {100, 0}, {150, 25}
        };
        LLine line;
        line.set_points(points, 4)
            .set_width(3)
            .set_color(0xFF0000)
            .set_pos(400, 290);
    }

    printf("14. LList\n");
    {
        LList list;
        list.set_pos(10, 350)
            .set_size(200, 150);
        list.add_text("Section 1");
        list.add_button(LV_SYMBOL_FILE, "Item 1");
        list.add_button(LV_SYMBOL_DIRECTORY, "Item 2");
        list.add_text("Section 2");
        list.add_button(LV_SYMBOL_OK, "Item 3");
    }

    printf("\n15. LTable\n");
    {
        LTable table;
        table.set_row_count(3)
             .set_column_count(3)
             .set_pos(230, 350)
             .set_size(250, 150);
        
        table.set_cell_value(0, 0, "Name");
        table.set_cell_value(0, 1, "Age");
        table.set_cell_value(0, 2, "City");
        
        table.set_cell_value(1, 0, "Alice");
        table.set_cell_value(1, 1, "25");
        table.set_cell_value(1, 2, "Beijing");
        
        table.set_cell_value(2, 0, "Bob");
        table.set_cell_value(2, 1, "30");
        table.set_cell_value(2, 2, "Shanghai");
        
        table.set_column_width(0, 80);
        table.set_column_width(1, 60);
        table.set_column_width(2, 100);
    }

    printf("16. LTabView\n");
    {
        LTabView tabview;
        tabview.set_pos(500, 350)
               .set_size(280, 200);
        
        auto tab1 = tabview.add_tab("Tab 1");
        auto tab2 = tabview.add_tab("Tab 2");
        auto tab3 = tabview.add_tab("Tab 3");
        
        LLabel label1;
        label1.set_text("Content of Tab 1");
        lv_obj_set_parent(label1.native(), tab1);
        
        LLabel label2;
        label2.set_text("Content of Tab 2");
        lv_obj_set_parent(label2.native(), tab2);
    }

    printf("17. LMenu\n");
    {
        LMenu menu;
        menu.set_pos(10, 520)
            .set_size(200, 150);
        
        auto page = menu.page_create("Settings");
        menu.set_page(page);
    }

    printf("18. LRoller\n");
    {
        LRoller roller;
        roller.set_options("January\nFebruary\nMarch\nApril\nMay\nJune")
              .set_selected(0)
              .set_visible_row_count(3)
              .set_pos(230, 520)
              .set_size(100, 150)
              .on_change([](lv_event_t* e) {
                  auto* roller = (lv_obj_t*)lv_event_get_target(e);
                  uint16_t idx = lv_roller_get_selected(roller);
                  printf("   Roller selected: %d\n", idx);
              });
    }

    printf("19. LScale\n");
    {
        LScale scale;
        scale.set_mode(LV_SCALE_MODE_HORIZONTAL_BOTTOM)
             .set_range(0, 100)
             .set_total_tick_count(11)
             .set_major_tick_every(5)
             .set_pos(350, 520)
             .set_size(200, 50);
    }

    printf("20. LSpan\n");
    {
        LSpan span;
        span.set_pos(350, 590)
            .set_size(200, 80)
            .set_align(LV_TEXT_ALIGN_LEFT)
            .set_mode(LV_SPAN_MODE_BREAK);
        
        auto s1 = span.new_span();
        lv_span_set_text(s1, "This is ");
        
        auto s2 = span.new_span();
        lv_span_set_text(s2, "rich text ");
        lv_style_set_text_color(&s2->style, lv_color_hex(0xFF0000));
        
        auto s3 = span.new_span();
        lv_span_set_text(s3, "example!");
    }

    printf("21. LSpinbox\n");
    {
        LSpinbox spinbox;
        spinbox.set_range(0, 100)
               .set_value(50)
               .set_step(5)
               .set_digit_format(3, 0)
               .set_pos(570, 520)
               .set_size(100, 40);
        
        LButton btn_inc;
        btn_inc.set_text("+")
               .set_size(40, 40)
               .set_pos(680, 520)
               .on_click([&spinbox](lv_event_t*) {
                   spinbox.increment();
               });
        
        LButton btn_dec;
        btn_dec.set_text("-")
               .set_size(40, 40)
               .set_pos(570, 570)
               .on_click([&spinbox](lv_event_t*) {
                   spinbox.decrement();
               });
    }

    printf("22. LButtonMatrix\n");
    {
        static const char* btnm_map[] = {"1", "2", "3", "\n",
                                         "4", "5", "6", "\n",
                                         "7", "8", "9", "\n",
                                         "0", LV_SYMBOL_BACKSPACE, ""};
        LButtonMatrix btnm;
        btnm.set_map(btnm_map)
            .set_pos(10, 690)
            .set_size(150, 150)
            .on_click([](lv_event_t* e) {
                auto* btnm = (lv_obj_t*)lv_event_get_target(e);
                uint16_t idx = lv_buttonmatrix_get_selected_button(btnm);
                printf("   Button matrix clicked: %d\n", idx);
            });
    }

    printf("23. LImageButton\n");
    {
        LImageButton imgbtn;
        imgbtn.set_pos(180, 690)
              .set_size(80, 80)
              .on_click([](lv_event_t*) {
                  printf("   Image button clicked!\n");
              });
    }

    printf("24. LMsgBox\n");
    {
        LButton show_msgbox;
        show_msgbox.set_text("Show MsgBox")
                   .set_pos(280, 690)
                   .set_size(120, 50)
                   .on_click([](lv_event_t*) {
                       LMsgBox msgbox(lv_scr_act());
                       msgbox.set_title("Information")
                             .set_text("This is a message box!")
                             .add_close_button();
                       auto btn = msgbox.add_footer_button("OK");
                       printf("   Message box shown\n");
                   });
    }

    printf("\n25. LCalendar\n");
    {
        LCalendar calendar;
        calendar.set_today_date(2026, 3, 15)
                .set_showed_date(2026, 3)
                .set_pos(420, 690)
                .set_size(250, 250);
    }

    printf("26. LChart\n");
    {
        LChart chart;
        chart.set_type(LV_CHART_TYPE_LINE)
             .set_point_count(10)
             .set_range(LV_CHART_AXIS_PRIMARY_Y, 0, 100)
             .set_pos(690, 690)
             .set_size(200, 150);
        
        auto series1 = chart.add_series(0xFF0000, LV_CHART_AXIS_PRIMARY_Y);
        auto series2 = chart.add_series(0x0000FF, LV_CHART_AXIS_PRIMARY_Y);
        
        for(int i = 0; i < 10; i++) {
            chart.set_next_value(series1, 20 + i * 5);
            chart.set_next_value(series2, 80 - i * 5);
        }
    }

    printf("27. LCanvas\n");
    {
        static uint8_t cbuf[LV_CANVAS_BUF_SIZE(100, 100, 32, LV_STRIDE_AUTO)];
        LCanvas canvas;
        canvas.set_buffer(cbuf, 100, 100, LV_COLOR_FORMAT_ARGB8888)
              .set_pos(910, 690)
              .set_size(100, 100);
        
        canvas.fill_bg(0xFFFFFF, LV_OPA_COVER);
        
        for(int i = 0; i < 100; i += 10) {
            canvas.set_px(i, i, 0xFF0000, LV_OPA_COVER);
            canvas.set_px(i, 99-i, 0x0000FF, LV_OPA_COVER);
        }
    }

    printf("28. LKeyboard\n");
    {
        LLineEdit ta_kb;
        ta_kb.set_pos(10, 960)
             .set_size(400, 80)
             .set_placeholder("Type with keyboard...");
        
        LKeyboard keyboard;
        keyboard.set_textarea(ta_kb.native())
                .set_pos(10, 1050)
                .set_size(400, 150);
    }

    printf("29. LAnimImage\n");
    {
        LAnimImage animimg;
        animimg.set_duration(1000)
               .set_repeat_count(LV_ANIM_REPEAT_INFINITE)
               .set_pos(430, 960)
               .set_size(100, 100);
    }

    printf("30. LTileView\n");
    {
        LTileView tileview;
        tileview.set_pos(550, 960)
                .set_size(300, 200);
        
        auto tile1 = tileview.add_tile(0, 0, LV_DIR_RIGHT);
        auto tile2 = tileview.add_tile(1, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
        auto tile3 = tileview.add_tile(2, 0, LV_DIR_LEFT);
        
        LLabel label_t1;
        label_t1.set_text("Tile 1");
        lv_obj_set_parent(label_t1.native(), tile1);
        lv_obj_center(label_t1.native());
        
        LLabel label_t2;
        label_t2.set_text("Tile 2");
        lv_obj_set_parent(label_t2.native(), tile2);
        lv_obj_center(label_t2.native());
        
        LLabel label_t3;
        label_t3.set_text("Tile 3");
        lv_obj_set_parent(label_t3.native(), tile3);
        lv_obj_center(label_t3.native());
    }

    printf("31. LWin\n");
    {
        LWin win;
        win.set_pos(870, 960)
           .set_size(300, 200);
        
        win.add_title("Window Title");
        auto close_btn = win.add_button(LV_SYMBOL_CLOSE, 40);
        
        auto content = win.get_content();
        LLabel label_win;
        label_win.set_text("Window content area");
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
