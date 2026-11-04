#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "easylvgl/core/platform.hpp"
#include <cstdio>

extern "C" {
#include "driver_backends.h"
}

/* 显示/输入后端由 port 的 backends 层负责，按可用后端顺序挑一个 */
static bool init_display_backend()
{
    static const char *candidates[] = {"SDL", "X11", "WAYLAND", "FBDEV", "DRM", "WIN32"};
    driver_backends_register();
    for(const char *name : candidates) {
        if(!driver_backends_is_supported(const_cast<char *>(name))) continue;
        if(driver_backends_init_backend(const_cast<char *>(name)) == 0) {
            printf("[easylvgl] display backend: %s\n", name);
            return true;
        }
    }
    return false;
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    lv_init();

    if(!init_display_backend()) {
        fprintf(stderr, "[easylvgl] no usable display backend\n");
        return 1;
    }

    lv_demo_widgets();

    while(true) {
        uint32_t idle = lv_timer_handler();
        easylvgl::sleep_ms(idle > 5 ? 5 : idle);
    }
    return 0;
}
