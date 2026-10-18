#include "lvgl/lvgl.h"
#include "easylvgl/lvgl_widgets.hpp"
#include "easylvgl/core/signal_handler.hpp"
#include "easylvgl/core/platform.hpp"
#include <cstdio>

using namespace easylvgl;

int main(void) {
    lv_init();
    lv_display_t* disp = lv_sdl_window_create(800, 600); (void)disp;
    lv_sdl_mouse_create();

    SignalHandler signal_handler;

    printf("=== LVGL++ Advanced Components Demo ===\n");
    printf("Press Ctrl+C to exit gracefully.\n\n");

    LLabel title;
    title.set_text("Advanced Components")
         .set_pos(10, 10)
         .set_color(0x2196F3);

    LTable table;
        table.set_row_count(4)
             .set_column_count(3)
             .set_size(300, 150);
    table.set_row_count(4)
         .set_column_count(3)
         .set_pos(10, 50)
         .set_size(300, 150);
    
    table.set_cell_value(0, 0, "Name");
    table.set_cell_value(0, 1, "Age");
    table.set_cell_value(0, 2, "City");
    
    table.set_cell_value(1, 0, "Alice");
    table.set_cell_value(1, 1, "25");
    table.set_cell_value(1, 2, "Beijing");
    
    table.set_cell_value(2, 0, "Bob");
    table.set_cell_value(2, 1, "30");
    table.set_cell_value(2, 2, "Shanghai");
    
    table.set_cell_value(3, 0, "Carol");
    table.set_cell_value(3, 1, "28");
    table.set_cell_value(3, 2, "Shenzhen");
    
    table.set_column_width(0, 100);
    table.set_column_width(1, 60);
    table.set_column_width(2, 120);

    LRoller roller;
    roller.set_options("January\nFebruary\nMarch\nApril\nMay\nJune")
          .set_selected(2)
          .set_visible_row_count(3)
          .set_pos(330, 50)
          .set_size(120, 150);

    LSpinbox spinbox;
    spinbox.set_range(0, 100)
           .set_value(50)
           .set_step(5)
           .set_digit_format(3, 0)
           .set_pos(10, 220)
           .set_size(120, 50);

    LButton btn_inc;
    btn_inc.set_text("+")
           .set_size(50, 50)
           .set_pos(140, 220)
           .on_click([&spinbox](lv_event_t*) {
               spinbox.increment();
               printf("Spinbox value: %d\n", spinbox.value());
           });

    LButton btn_dec;
    btn_dec.set_text("-")
           .set_size(50, 50)
           .set_pos(200, 220)
           .on_click([&spinbox](lv_event_t*) {
               spinbox.decrement();
               printf("Spinbox value: %d\n", spinbox.value());
           });

    static const char* btnm_map[] = {
        "1", "2", "3", "\n",
        "4", "5", "6", "\n",
        "7", "8", "9", "\n",
        "0", LV_SYMBOL_BACKSPACE, ""
    };

    LButtonMatrix btnm;
    btnm.set_map(btnm_map)
        .set_pos(270, 220)
        .set_size(180, 180);

    LChart chart;
    chart.set_type(LV_CHART_TYPE_LINE)
         .set_point_count(10)
         .set_range(LV_CHART_AXIS_PRIMARY_Y, 0, 100)
         .set_pos(10, 290)
         .set_size(250, 150);
    
    auto series1 = chart.add_series(0xFF0000, LV_CHART_AXIS_PRIMARY_Y);
    auto series2 = chart.add_series(0x0000FF, LV_CHART_AXIS_PRIMARY_Y);
    
    for(int i = 0; i < 10; i++) {
        chart.set_next_value(series1, 20 + i * 5);
        chart.set_next_value(series2, 80 - i * 5);
    }

    printf("\nDemo running. Press Ctrl+C to exit gracefully.\n");

    while(signal_handler.is_running()) {
        lv_timer_handler();
        easylvgl::sleep_ms(5);
    }

    printf("\nCleaning up resources...\n");
    return 0;
}
