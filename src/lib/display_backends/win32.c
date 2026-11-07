/**
 * @file win32.c
 *
 * LVGL 原生 Win32 显示/输入后端（Windows 桌面，无需 SDL）
 */

#include "lvgl/lvgl.h"

#if LV_USE_WINDOWS

#include <stdlib.h>

#include "../simulator_util.h"
#include "../simulator_settings.h"
#include "../backends.h"

extern simulator_settings_t settings;

static lv_display_t * init_win32(void);
static char * backend_name = "WIN32";

/**
 * Register the backend
 * @param backend the backend descriptor
 */
int backend_init_win32(backend_t * backend)
{
    LV_ASSERT_NULL(backend);

    backend->handle->display = malloc(sizeof(display_backend_t));
    LV_ASSERT_NULL(backend->handle->display);

    backend->handle->display->init_display = init_win32;
    backend->name = backend_name;
    backend->type = BACKEND_DISPLAY;

    return 0;
}

static lv_display_t * init_win32(void)
{
    lv_display_t * disp = lv_windows_create_display(
        L"EasyLVGL", (int32_t)settings.window_width, (int32_t)settings.window_height, 100, false,
        true);

    if(disp == NULL) {
        return NULL;
    }

    lv_windows_acquire_pointer_indev(disp);
    lv_windows_acquire_keypad_indev(disp);

    return disp;
}

#endif /* LV_USE_WINDOWS */
