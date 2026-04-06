#include "lvgl++/lvgl_widgets.hpp"
#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include <unistd.h>
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

    printf("=== LVGL++ Layer 2 Demo ===\n");

    LVBoxLayout main_container;
    main_container.set_spacing(20)
                  .set_padding(30)
                  .set_size(400, 500)
                  .set_align(LV_ALIGN_CENTER);

    LLabel title(main_container.native());
    title.set_text("LVGL++ Components")
         .set_color(0x2196F3)
         .set_font_size(24);

    LLabel subtitle(main_container.native());
    subtitle.set_text("Modern C++ UI Framework")
            .set_color(0x666666)
            .set_font_size(14);

    LHBoxLayout button_row(main_container.native());
    button_row.set_spacing(10);

    LButton btn1(button_row.native());
    btn1.set_text("Primary")
        .set_size(100, 40)
        .primary()
        .on_click([](auto*) {
            printf("Primary button clicked!\n");
        });

    LButton btn2(button_row.native());
    btn2.set_text("Success")
        .set_size(100, 40)
        .success()
        .on_click([](auto*) {
            printf("Success button clicked!\n");
        });

    LButton btn3(button_row.native());
    btn3.set_text("Danger")
        .set_size(100, 40)
        .danger()
        .on_click([](auto*) {
            printf("Danger button clicked!\n");
        });

    LHBoxLayout button_row2(main_container.native());
    button_row2.set_spacing(10);

    LButton btn4(button_row2.native());
    btn4.set_text("Warning")
        .set_size(100, 40)
        .warning()
        .on_click([](auto*) {
            printf("Warning button clicked!\n");
        });

    LButton btn5(button_row2.native());
    btn5.set_text("Disabled")
        .set_size(100, 40)
        .set_disabled();

    static int counter = 0;
    LLabel counter_label(main_container.native());
    counter_label.set_text_fmt("Counter: %d", counter)
                 .set_color(0x333333);

    LButton increment_btn(main_container.native());
    increment_btn.set_text("Increment Counter")
                 .set_size(200, 50)
                 .primary()
                 .on_click([&counter_label](auto*) {
                     counter++;
                     lv_label_set_text_fmt(counter_label.native(), "Counter: %d", counter);
                     printf("Counter: %d\n", counter);
                 });

    LLabel info(main_container.native());
    info.set_text("Click buttons to test!")
        .set_color(0x999999)
        .set_font_size(12);

    printf("Layer 2 demo started!\n");
    printf("Features:\n");
    printf("- LButton component with themes\n");
    printf("- LLabel component with colors\n");
    printf("- LVBoxLayout/LHBoxLayout layouts\n");
    printf("- Event handling\n");

    while(1) {
        lv_timer_handler();
        usleep(5000);
    }
    
    return 0;
}
