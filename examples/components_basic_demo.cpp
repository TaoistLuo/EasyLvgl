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

    printf("=== LVGL++ Basic Components Demo ===\n");
    printf("Press Ctrl+C to exit gracefully.\n\n");

    LLabel title;
    title.text("LVGL++ Components Demo")
         .pos(10, 10)
         .color(0x2196F3);

    LButton btn1;
    btn1.text("Primary Button")
        .size(150, 50)
        .pos(10, 50)
        .primary()
        .on_click([](lv_event_t*) {
            printf("Primary button clicked!\n");
        });

    LButton btn2;
    btn2.text("Secondary")
        .size(150, 50)
        .pos(170, 50)
        .on_click([](lv_event_t*) {
            printf("Secondary button clicked!\n");
        });

    LSlider slider;
    slider.range(0, 100)
          .value(50)
          .size(310, 10)
          .pos(10, 120)
          .on_change([](lv_event_t* e) {
              auto* obj = (lv_obj_t*)lv_event_get_target(e);
              int val = lv_slider_get_value(obj);
              printf("Slider value: %d\n", val);
          });

    LCheckbox cb;
    cb.text("Enable Feature")
      .pos(10, 150)
      .checked(true)
      .on_change([](lv_event_t* e) {
          auto* obj = (lv_obj_t*)lv_event_get_target(e);
          bool checked = lv_obj_has_state(obj, LV_STATE_CHECKED);
          printf("Checkbox: %s\n", checked ? "checked" : "unchecked");
      });

    LSwitch sw;
    sw.pos(200, 150)
      //.on()
      .on_change([](lv_event_t* e) {
          auto* obj = (lv_obj_t*)lv_event_get_target(e);
          bool on = lv_obj_has_state(obj, LV_STATE_CHECKED);
          printf("Switch: %s\n", on ? "ON" : "OFF");
      });

    LBar bar;
    bar.range(0, 100)
       .value(75, LV_ANIM_ON)
       .size(310, 20)
       .pos(10, 200);

    LArc arc;
    arc.range(0, 100)
       .value(60)
       .rotation(270)
       .size(100)
       .pos(10, 240);

    LSpinner spinner;
    spinner.size(50)
           .pos(130, 265);

    LLed led_green;
    led_green.color(0x00FF00)
             .brightness(255)
             //.on()
             .size(30)
             .pos(200, 270);

    LLed led_red;
    led_red.color(0xFF0000)
           .brightness(200)
           .off()
           .size(30)
           .pos(250, 270);

    while(signal_handler.is_running()) {
        lv_timer_handler();
        usleep(5000);
    }

    printf("\nCleaning up resources...\n");
    return 0;
}
