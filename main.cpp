#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "easylvgl/core/platform.hpp"
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ? : dflt;
}

#if LV_USE_LINUX_FBDEV
static void lv_linux_disp_init(void)
{
    const char *device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
    lv_display_t * disp = lv_linux_fbdev_create();

    lv_linux_fbdev_set_file(disp, device);
}
#elif LV_USE_LINUX_DRM
static void lv_linux_disp_init(void)
{
    const char *device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    lv_display_t * disp = lv_linux_drm_create();

    lv_linux_drm_set_file(disp, device, -1);
}
#elif LV_USE_SDL
static void lv_linux_disp_init(void)
{
    const int width = atoi(getenv("LV_SDL_VIDEO_WIDTH") ? : "1280");
    const int height = atoi(getenv("LV_SDL_VIDEO_HEIGHT") ? : "720");

    lv_sdl_window_create(width, height);
}
#else
#error Unsupported configuration
#endif

// Old LObject/LStyle removed - use new LVGL++ API
// #include "easylvgl/lvgl_widgets.hpp"

int main(void)
{
    lv_init();

    /*Linux display device init*/
    lv_linux_disp_init();
    //初始化触摸
    // lv_indev_t * touch;
    // touch = lv_evdev_create(LV_INDEV_TYPE_POINTER,"/dev/input/event4");

    /*Create a Demo*/
    lv_demo_widgets();
    // lv_demo_widgets_start_slideshow();

    /*Handle LVGL tasks*/
    lv_sdl_mouse_create();
    
    // Old LObject/LStyle code removed
    // To see new LVGL++ API examples, run:
    //   ./layer1_demo
    //   ./layer2_demo
    //   ./complete_demo
    while(1) {
        lv_timer_handler();
        easylvgl::sleep_ms(5);
    }
    return 0;
}
