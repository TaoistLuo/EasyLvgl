/*
 * 设计器自测：加载 .lui -> 用真实 LvglRenderer 建树 -> 回读样式值 + 帧缓冲哈希。
 * 由 scripts/designer/matrix_all.py 驱动，逐组件、逐属性比较两个取值是否生效。
 */
#include "lvgl/lvgl.h"
#include <lvgl/src/libs/fsdrv/lv_fsdrv.h>
#include "model/LuiDocument.hpp"
#include "canvas/LvglRenderer.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace {

constexpr int kWidth = 800;
constexpr int kHeight = 480;

std::vector<uint32_t> g_framebuffer(kWidth * kHeight);

void flushCb(lv_display_t* disp, const lv_area_t* area, uint8_t* pxMap) {
    const int w = lv_area_get_width(area);
    const int h = lv_area_get_height(area);
    for (int y = 0; y < h; y++) {
        std::memcpy(&g_framebuffer[(size_t)(area->y1 + y) * kWidth + area->x1],
                    pxMap + (size_t)y * w * 4, (size_t)w * 4);
    }
    lv_display_flush_ready(disp);
}

uint64_t hashFramebuffer() {
    uint64_t h = 1469598103934665603ull;
    const auto* p = reinterpret_cast<const uint8_t*>(g_framebuffer.data());
    for (size_t i = 0; i < g_framebuffer.size() * sizeof(uint32_t); i++) {
        h ^= p[i];
        h *= 1099511628211ull;
    }
    return h;
}

} // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: designer_matrix <file.lui> [id] [prop]\n");
        return 2;
    }

    lv_init();
    lv_fs_stdio_init();

    static lv_color_t drawBuf[kWidth * 40];
    lv_display_t* disp = lv_display_create(kWidth, kHeight);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888);
    lv_display_set_buffers(disp, drawBuf, nullptr, sizeof(drawBuf), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, flushCb);

    const std::string arg1 = argv[1];
    if (arg1 == "--label") {
        lv_obj_t* l = lv_label_create(lv_scr_act());
        lv_label_set_text(l, "HELLO");
        lv_obj_set_pos(l, 10, 10);
        lv_obj_update_layout(lv_scr_act());
        lv_obj_invalidate(lv_scr_act());
        lv_refr_now(disp);
        lv_refr_now(disp);
        std::printf("hash %016llx\n", (unsigned long long)hashFramebuffer());
        return 0;
    }
    if (arg1 == "--qrcode") {
        lv_obj_t* q = lv_qrcode_create(lv_scr_act());
        lv_obj_set_pos(q, 10, 10);
        lv_qrcode_set_size(q, 160);
        lv_obj_set_size(q, 160, 160);
        lv_qrcode_set_dark_color(q, lv_color_black());
        lv_qrcode_set_light_color(q, lv_color_white());
        lv_result_t qrres = lv_qrcode_update(q, "https://lvgl.io", 15);
        std::printf("qr_update %d\n", (int)qrres);
        std::printf("qr_size %dx%d valid %d\n", (int)lv_obj_get_width(q), (int)lv_obj_get_height(q), (int)lv_qrcode_is_render_valid(q));
        lv_obj_update_layout(lv_scr_act());
        lv_refr_now(disp);
        std::printf("hash %016llx\n", (unsigned long long)hashFramebuffer());
        return 0;
    }
    if (arg1 == "--barcode") {
        lv_obj_t* b = lv_barcode_create(lv_scr_act());
        lv_obj_set_pos(b, 10, 10);
        lv_obj_set_size(b, 300, 100);
        lv_barcode_set_dark_color(b, lv_color_black());
        lv_barcode_set_light_color(b, lv_color_white());
        lv_result_t bcres = lv_barcode_update(b, "https://lvgl.io");
        std::printf("bc_update %d\n", (int)bcres);
        lv_barcode_render(b);
        std::printf("bc_size %dx%d valid %d\n", (int)lv_obj_get_width(b), (int)lv_obj_get_height(b), (int)lv_barcode_is_render_valid(b));
        lv_obj_update_layout(lv_scr_act());
        lv_refr_now(disp);
        std::printf("hash %016llx\n", (unsigned long long)hashFramebuffer());
        return 0;
    }

    designer::LuiDocument doc;
    if (!doc.load(QString::fromLocal8Bit(argv[1]))) {
        std::fprintf(stderr, "load failed: %s\n", argv[1]);
        return 3;
    }

    designer::LvglRenderer renderer;
    renderer.buildTree(doc);

    lv_obj_update_layout(lv_scr_act());
    lv_refr_now(disp);

    const std::string id = argc > 2 ? argv[2] : "w";
    const std::string prop = argc > 3 ? argv[3] : "";

    std::printf("found %d\n", renderer.findObject(id) ? 1 : 0);
    if (!prop.empty() && renderer.findObject(id)) {
        for (int pass = 0; pass < 2; pass++) {
            QVariant v = renderer.styleValue(id, prop);
            if (!v.isValid()) {
                std::printf("style <none>\n");
                break;
            }
            std::printf("style %s\n", v.toString().toStdString().c_str());
        }
    }
    std::printf("type %s\n", renderer.objectType(id).c_str());
    std::printf("hash %016llx\n", (unsigned long long)hashFramebuffer());
    return 0;
}
