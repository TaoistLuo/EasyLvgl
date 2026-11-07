#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "easylvgl/core/platform.hpp"
#include <cstdio>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    lv_init();

    if(easylvgl::init_desktop_display(800, 480) == nullptr) {
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
