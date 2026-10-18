/* 图片资源编译期嵌入示例（等价 Qt 的 qrc）
 *
 * screen.lui 里：
 *   "resources": { "caret": "assets/caret_down.png" }
 *   LImage 的 "src": "#caret"
 *
 * scripts/assets/convert_assets.py 把 PNG 转成 lv_image_dsc_t 常量，
 * luic 生成 set_src(&caret) —— 最终固件里没有图片文件，也没有 lv_fs 依赖。
 */
#include "lvgl++/lvgl_widgets.hpp"
#include "lvgl++/core/signal_handler.hpp"
#include "screen_ui.hpp"
#include <unistd.h>
#include <cstdio>

using namespace lvgl;

#if LV_USE_SDL
static void lv_linux_disp_init(void) {
    lv_sdl_window_create(EmbeddedImage_UI::kScreenWidth, EmbeddedImage_UI::kScreenHeight);
}
#else
#error Unsupported configuration
#endif

int main(void) {
    lv_init();
    lv_linux_disp_init();
    lv_sdl_mouse_create();

    SignalHandler signal_handler;

    printf("=== 图片嵌入示例 ===\n");
    printf("caret_down.png 已转换为 lv_image_dsc_t，代码里直接用 &caret，\n");
    printf("运行时不读取任何图片文件（可用 lsof 验证）。Ctrl+C 退出。\n\n");

    EmbeddedImage_UI ui;
    ui.setup();
    ui.bind_events();

    while (signal_handler.is_running()) {
        lv_timer_handler();
        usleep(5000);
    }
    printf("\n退出。\n");
    return 0;
}
