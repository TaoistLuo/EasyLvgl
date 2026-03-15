#include "lvgl++/lvgl_widgets.hpp"
#include "lvgl++/core/signal_handler.hpp"
#include "main_screen_ui.hpp"
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
    const int height = atoi(getenv("LV_SDL_VIDEO_HEIGHT") ? : "480");
    lv_sdl_window_create(width, height);
}
#else
#error Unsupported configuration
#endif

class MainScreen : public MainScreen_UI {
public:
    void on_btn_login_clicked(lv_event_t* e) override {
        static int count = 0;
        count++;
        printf("Login clicked! count=%d\n", count);
    }

    void on_brightness_value_changed(lv_event_t* e) override {
        int val = lv_slider_get_value(m_brightness->native());
        printf("Brightness: %d\n", val);
        m_progress->set_value(val);
    }

    void on_cb_enable_value_changed(lv_event_t* e) override {
        bool checked = lv_obj_has_state(m_cb_enable->native(), LV_STATE_CHECKED);
        printf("Feature: %s\n", checked ? "ON" : "OFF");
    }
};

int main(void)
{
    lv_init();
    lv_linux_disp_init();
    lv_sdl_mouse_create();

    SignalHandler signal_handler;

    printf("=== LVGL++ luic End-to-End Demo ===\n");
    printf("UI generated from main_screen.lui\n");
    printf("Press Ctrl+C to exit gracefully.\n\n");

    MainScreen screen;
    screen.setup();
    screen.bind_events();

    while(signal_handler.is_running()) {
        lv_timer_handler();
        usleep(5000);
    }

    printf("\nCleaning up resources...\n");
    return 0;
}
