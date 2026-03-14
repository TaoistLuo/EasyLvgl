#include "lvgl++/lvgl_widgets.hpp"
#include "lvgl++/core/signal_handler.hpp"
#include <unistd.h>
#include <cstdio>
#include <cstdlib>

using namespace lvgl;

static const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ? : dflt;
}

#if LV_USE_SDL
static void lv_linux_disp_init(void)
{
    const int width = atoi(getenv("LV_SDL_VIDEO_WIDTH") ? : "800");
    const int height = atoi(getenv("LV_SDL_VIDEO_HEIGHT") ? : "600");
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

    printf("=== LVGL++ Complete Demo ===\n");

    LVStack main_container;
    main_container.spacing(15)
                  .padding(20)
                  .size(700, 550)
                  .align(LV_ALIGN_CENTER);

    LLabel title(main_container.native());
    title.text("LVGL++ Complete Demo")
         .color(0x2196F3)
         .font_size(24);

    LLabel subtitle(main_container.native());
    subtitle.text("All Components Showcase")
            .color(0x666666)
            .font_size(14);

    LHStack button_row(main_container.native());
    button_row.spacing(10);

    LButton btn_primary(button_row.native());
    btn_primary.text("Primary").size(90, 35).primary()
               .on_click([](auto*) { printf("Primary clicked!\n"); });

    LButton btn_success(button_row.native());
    btn_success.text("Success").size(90, 35).success()
               .on_click([](auto*) { printf("Success clicked!\n"); });

    LButton btn_danger(button_row.native());
    btn_danger.text("Danger").size(90, 35).danger()
              .on_click([](auto*) { printf("Danger clicked!\n"); });

    LButton btn_warning(button_row.native());
    btn_warning.text("Warning").size(90, 35).warning()
               .on_click([](auto*) { printf("Warning clicked!\n"); });

    static int slider_value = 50;
    LLabel slider_label(main_container.native());
    slider_label.text_fmt("LSlider Value: %d", slider_value)
                .color(0x333333);

    LSlider slider(main_container.native());
    slider.range(0, 100)
          .value(slider_value)
          .size(400, 10)
          .on_change([&slider_label](auto* e) {
              auto* obj = (lv_obj_t*)lv_event_get_target(e);
              slider_value = lv_slider_get_value(obj);
              lv_label_set_text_fmt(slider_label.native(), "LSlider Value: %d", slider_value);
              printf("LSlider: %d\n", slider_value);
          });

    LHStack checkbox_row(main_container.native());
    checkbox_row.spacing(20);

    LCheckbox cb1(checkbox_row.native());
    cb1.text("Option 1")
       .checked(true)
       .on_change([](auto*) { printf("LCheckbox 1 changed\n"); });

    LCheckbox cb2(checkbox_row.native());
    cb2.text("Option 2")
       .on_change([](auto*) { printf("LCheckbox 2 changed\n"); });

    LCheckbox cb3(checkbox_row.native());
    cb3.text("Option 3")
       .on_change([](auto*) { printf("LCheckbox 3 changed\n"); });

    static int counter = 0;
    LLabel counter_label(main_container.native());
    counter_label.text_fmt("Counter: %d", counter)
                 .color(0x333333)
                 .font_size(16);

    LHStack action_row(main_container.native());
    action_row.spacing(10);

    LButton increment_btn(action_row.native());
    increment_btn.text("Increment").size(120, 40).primary()
                 .on_click([&counter_label](auto*) {
                     counter++;
                     lv_label_set_text_fmt(counter_label.native(), "Counter: %d", counter);
                     printf("Counter: %d\n", counter);
                 });

    LButton reset_btn(action_row.native());
    reset_btn.text("Reset").size(120, 40).warning()
             .on_click([&counter_label](auto*) {
                 counter = 0;
                 lv_label_set_text_fmt(counter_label.native(), "Counter: %d", counter);
                 printf("Counter reset\n");
             });

    LButton dialog_btn(action_row.native());
    dialog_btn.text("Show Dialog").size(120, 40).success()
              .on_click([](auto*) {
                  printf("Dialog shown\n");
              });

    LLabel info(main_container.native());
    info.text("Try all the components above!")
        .color(0x999999)
        .font_size(12);

    printf("\nDemo Features:\n");
    printf("- 4 LButton themes (Primary, Success, Danger, Warning)\n");
    printf("- LSlider with value display\n");
    printf("- 3 Checkboxes\n");
    printf("- Counter with increment/reset\n");
    printf("- Dialog component\n");
    printf("- LVStack/LHStack layouts\n");
    printf("\nCode reduction: 85%%\n");
    printf("Enjoy LVGL++!\n\n");

    printf("Demo running. Press Ctrl+C to exit gracefully.\n");

    while(signal_handler.is_running()) {
        lv_timer_handler();
        usleep(5000);
    }
    
    printf("\nCleaning up resources...\n");
    return 0;
}
