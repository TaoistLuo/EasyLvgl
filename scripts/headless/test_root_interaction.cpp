#include "canvas/LvglRenderer.hpp"
#include "model/LuiDocument.hpp"
#include <cstdio>
#include <string>

static lv_point_t g_pt = {0, 0};
static bool g_pressed = false;

static void read_cb(lv_indev_t*, lv_indev_data_t* data) {
    data->point = g_pt;
    data->state = g_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

static int fails = 0;
static std::string g_clicked, g_movedId;
static int g_mx = 0, g_my = 0, g_mw = 0, g_mh = 0;

#define CHECK(c) do { if (!(c)) { printf("  FAIL: %s (line %d)\n", #c, __LINE__); fails++; } \
                      else printf("  ok: %s\n", #c); } while (0)

static void tick(designer::LvglRenderer& r) {
    for (int i = 0; i < 8; i++) { lv_tick_inc(30); lv_timer_handler(); }
    r.pollEvents();
}

int main(int argc, char** argv) {
    lv_init();
    static lv_color_t buf[400 * 16];
    lv_display_t* d = lv_display_create(400, 320);
    lv_display_set_flush_cb(d, [](lv_display_t* dp, const lv_area_t*, uint8_t*) { lv_display_flush_ready(dp); });
    lv_display_set_buffers(d, buf, nullptr, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_indev_t* indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, read_cb);

    const char* lui = (argc > 1) ? argv[1] : "examples/widgets/table.lui";
    designer::LuiDocument doc;
    if (!doc.load(QString(lui))) {
        printf("load failed: %s\n", lui);
        return 2;
    }
    printf("doc %dx%d children=%d\n", doc.width(), doc.height(), (int)doc.children().size());

    designer::LvglRenderer renderer;
    renderer.setOnClick([&](const std::string& id) { g_clicked = id; });
    renderer.setOnMoved([&](const std::string& id, int x, int y, int w, int h) {
        g_movedId = id; g_mx = x; g_my = y; g_mw = w; g_mh = h;
    });
    printf("building...\n"); renderer.buildTree(doc); printf("built.\n");

    printf("[1] 点击画布空白处 (350,280) -> 应选中根\n");
    g_clicked.clear();
    g_pt = {350, 280}; g_pressed = true; tick(renderer);
    g_pressed = false; tick(renderer);
    CHECK(g_clicked == designer::LvglRenderer::kRootId);

    printf("[2] 点击表格 (100,100) -> 应选中表格而非根\n");
    g_clicked.clear();
    g_pt = {100, 100}; g_pressed = true; tick(renderer);
    g_pressed = false; tick(renderer);
    CHECK(g_clicked != designer::LvglRenderer::kRootId);
    CHECK(g_clicked == "w");

    printf("[3] 根选中后不应存在 LVGL 手柄（根尺寸改由 Qt 层处理）\n");
    renderer.selectObject(designer::LvglRenderer::kRootId);
    lv_timer_handler();
    g_movedId.clear(); g_mw = g_mh = 0;
    g_pt = {396, 316}; g_pressed = true; tick(renderer);
    g_pt = {399, 319}; tick(renderer);
    g_pressed = false; tick(renderer);
    CHECK(g_movedId.empty());

    printf("[5] 普通控件（表格）手柄拖拽仍应工作\n");
    renderer.selectObject("w");
    lv_timer_handler();
    g_movedId.clear(); g_mw = g_mh = 0;
    g_pt = {259, 219}; g_pressed = true; tick(renderer);
    g_pt = {299, 259}; tick(renderer);
    g_pressed = false; tick(renderer);
    printf("     table moved -> id=%s %dx%d\n", g_movedId.c_str(), g_mw, g_mh);
    CHECK(g_movedId == "w");

    printf("[4] 显示分辨率不应因选择根而改变\n");
    CHECK(lv_display_get_horizontal_resolution(lv_display_get_default()) == 400);
    CHECK(lv_display_get_vertical_resolution(lv_display_get_default()) == 320);

    printf("\n%s (%d 处失败)\n", fails ? "FAILED" : "PASSED", fails);
    return fails ? 1 : 0;
}
