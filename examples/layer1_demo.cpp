#include "lvgl++/lvgl_core.hpp"
#include "lvgl++/core/signal_handler.hpp"
#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl++/core/platform.hpp"
#include <stdio.h>

using namespace lvgl;

static const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ? : dflt;
}

#if LV_USE_SDL
static void lv_linux_disp_init(void)
{
    const int width = atoi(getenv("LV_SDL_VIDEO_WIDTH") ? : "800");
    const int height = atoi(getenv("LV_SDL_VIDEO_HEIGHT") ? : "480");
    lv_sdl_window_create(width, height);
}
#else
#error Unsupported configuration
#endif

int main(void)
{
    lv_init();
    lv_linux_disp_init();
    lv_sdl_mouse_create();

    SignalHandler signal_handler;

    printf("=== LVGL++ Layer 1 Demo ===\n");
    printf("Press Ctrl+C to exit gracefully.\n\n");

    auto btn = configure(lv_button_create(lv_screen_active()))
        .size(120, 50)
        .pos(10, 50)
        .get();

    auto label = configure(lv_label_create(btn))
        .get();
    lv_obj_center(label);
    lv_label_set_text(label, "Button");

    connect_event(btn, LV_EVENT_CLICKED, [](lv_event_t* e) {
        printf("Button clicked!\n");
    });

    LStyle default_style;
    default_style.set_bg_color(0x2196F3)
                 .set_border_width(2)
                 .set_border_color(0x1976D2)
                 .set_radius(5)
                 .apply_to(btn);

    LStyle pressed_style;
    pressed_style.set_bg_color(0x1976D2).apply_to(btn, LV_STATE_PRESSED);

    auto label2 = configure(lv_label_create(lv_scr_act()))
        .pos(10, 10)
        .get();
    lv_label_set_text(label2, "LVGL++ Layer 1 Demo");
    
    printf("Demo running. Press Ctrl+C to exit gracefully.\n");

    while(signal_handler.is_running()) {
        lv_timer_handler();
        lvgl::sleep_ms(5);
    }

    printf("\nCleaning up resources...\n");
    return 0;
}
