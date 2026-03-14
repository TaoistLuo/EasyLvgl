#include "lvgl/lvgl.h"
#include "lvgl++/lvgl_widgets.hpp"
#include "lvgl++/core/signal_handler.hpp"
#include <unistd.h>
#include <cstdio>

using namespace lvgl;

int main(void) {
    lv_init();
    lv_display_t* disp = lv_sdl_window_create(800, 600); (void)disp;
    lv_sdl_mouse_create();

    SignalHandler signal_handler;

    printf("=== LVGL++ Advanced Components Demo ===\n");
    printf("Press Ctrl+C to exit gracefully.\n\n");

    LLabel title;
    title.text("Advanced Components")
         .pos(10, 10)
         .color(0x2196F3);

    LTable table;
    table.row_count(4)
         .column_count(3)
         .pos(10, 50)
         .size(300, 150);
    
    table.cell_value(0, 0, "Name");
    table.cell_value(0, 1, "Age");
    table.cell_value(0, 2, "City");
    
    table.cell_value(1, 0, "Alice");
    table.cell_value(1, 1, "25");
    table.cell_value(1, 2, "Beijing");
    
    table.cell_value(2, 0, "Bob");
    table.cell_value(2, 1, "30");
    table.cell_value(2, 2, "Shanghai");
    
    table.cell_value(3, 0, "Carol");
    table.cell_value(3, 1, "28");
    table.cell_value(3, 2, "Shenzhen");
    
    table.column_width(0, 100);
    table.column_width(1, 60);
    table.column_width(2, 120);

    LRoller roller;
    roller.options("January\nFebruary\nMarch\nApril\nMay\nJune")
          .selected(2)
          .visible_row_count(3)
          .pos(330, 50)
          .size(120, 150);

    LSpinbox spinbox;
    spinbox.range(0, 100)
           .value(50)
           .step(5)
           .digit_format(3, 0)
           .pos(10, 220)
           .size(120, 50);

    LButton btn_inc;
    btn_inc.text("+")
           .size(50, 50)
           .pos(140, 220)
           .on_click([&spinbox](lv_event_t*) {
               spinbox.increment();
               printf("Spinbox value: %d\n", spinbox.value());
           });

    LButton btn_dec;
    btn_dec.text("-")
           .size(50, 50)
           .pos(200, 220)
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
    btnm.map(btnm_map)
        .pos(270, 220)
        .size(180, 180);

    LChart chart;
    chart.type(LV_CHART_TYPE_LINE)
         .point_count(10)
         .range(LV_CHART_AXIS_PRIMARY_Y, 0, 100)
         .pos(10, 290)
         .size(250, 150);
    
    auto series1 = chart.add_series(0xFF0000, LV_CHART_AXIS_PRIMARY_Y);
    auto series2 = chart.add_series(0x0000FF, LV_CHART_AXIS_PRIMARY_Y);
    
    for(int i = 0; i < 10; i++) {
        chart.set_next_value(series1, 20 + i * 5);
        chart.set_next_value(series2, 80 - i * 5);
    }

    printf("\nDemo running. Press Ctrl+C to exit gracefully.\n");

    while(signal_handler.is_running()) {
        lv_timer_handler();
        usleep(5000);
    }

    printf("\nCleaning up resources...\n");
    return 0;
}
