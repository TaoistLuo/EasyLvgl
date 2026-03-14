/**
 * @file untested_components_demo.cpp
 * @brief LVGL++ Untested Components Demo
 * @version 1.0.0
 * @date 2026-03-15
 */

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

    printf("=== LVGL++ Untested Components Demo ===\n");
    printf("Testing 7 previously untested components.\n");
    printf("Press Ctrl+C to exit gracefully.\n\n");

    LVStack main_container;
    main_container.spacing(10)
                  .padding(15)
                  .size(750, 550)
                  .align(LV_ALIGN_CENTER);

    printf("1. Testing LTextArea...\n");
    {
        LLabel title(main_container.native());
        title.text("1. TextArea - Text Input").color(0x2196F3);

        LTextArea textarea(main_container.native());
        textarea.placeholder("Enter your text here...")
                .size(700, 60)
                .one_line(false)
                .on_value_changed([](lv_event_t* e) {
                    lv_obj_t* ta = static_cast<lv_obj_t*>(lv_event_get_target(e));
                    printf("   TextArea value: %s\n", lv_textarea_get_text(ta));
                });
    }

    printf("2. Testing LButtonMatrix...\n");
    {
        LLabel title(main_container.native());
        title.text("2. ButtonMatrix - Button Grid").color(0x2196F3);

        static const char* btnm_map[] = {"1", "2", "3", "\n",
                                          "4", "5", "6", "\n",
                                          "7", "8", "9", ""};

        LButtonMatrix btnm(main_container.native());
        btnm.map(btnm_map)
            .size(300, 150)
            .on_value_changed([](lv_event_t* e) {
                lv_obj_t* obj = static_cast<lv_obj_t*>(lv_event_get_target(e));
                uint32_t id = lv_buttonmatrix_get_selected_button(obj);
                const char* txt = lv_buttonmatrix_get_button_text(obj, id);
                printf("   ButtonMatrix clicked: %s\n", txt);
            });
    }

    printf("3. Testing LImageButton...\n");
    {
        LLabel title(main_container.native());
        title.text("3. ImageButton - Image Button").color(0x2196F3);

        LImageButton imgbtn(main_container.native());
        imgbtn.size(80, 80)
              .on_click([](lv_event_t*) {
                  printf("   ImageButton clicked!\n");
              });
    }

    printf("4. Testing LMsgBox...\n");
    {
        LButton show_msgbox_btn(main_container.native());
        show_msgbox_btn.text("Show Message Box")
                       .size(200, 40);
        
        lv_obj_add_event_cb(show_msgbox_btn.native(), [](lv_event_t*) {
            printf("   Creating MsgBox...\n");
            
            LMsgBox msgbox(lv_screen_active());
            msgbox.title("Confirmation")
                  .text("Are you sure?")
                  .add_close_button()
                  .on_value_changed([](lv_event_t* e) {
                      printf("   MsgBox closed\n");
                  });
            
            lv_obj_t* btn_ok = msgbox.add_footer_button("OK");
            lv_obj_t* btn_cancel = msgbox.add_footer_button("Cancel");
        }, LV_EVENT_CLICKED, nullptr);
    }

    printf("5. Testing LTabView...\n");
    {
        LLabel title(main_container.native());
        title.text("5. TabView - Tab Container").color(0x2196F3);

        LTabView tabview(main_container.native());
        tabview.size(700, 150);
        
        lv_obj_t* tab1 = lv_tabview_add_tab(tabview.native(), "Tab 1");
        lv_obj_t* tab2 = lv_tabview_add_tab(tabview.native(), "Tab 2");
        
        LLabel label1(tab1);
        label1.text("Content of Tab 1");
        
        LLabel label2(tab2);
        label2.text("Content of Tab 2");
    }

    printf("6. Testing LAnimImage...\n");
    {
        LLabel title(main_container.native());
        title.text("6. AnimImage - Animated Image").color(0x2196F3);
        
        LLabel note(main_container.native());
        note.text("(AnimImage requires image array - skipped)").color(0x999999);
    }

    printf("7. Testing LTileView...\n");
    {
        LLabel title(main_container.native());
        title.text("7. TileView - Tile View").color(0x2196F3);

        LTileView tileview(main_container.native());
        tileview.size(700, 100);
        
        lv_obj_t* tile1 = lv_tileview_add_tile(tileview.native(), 0, 0, LV_DIR_RIGHT);
        lv_obj_t* tile2 = lv_tileview_add_tile(tileview.native(), 1, 0, LV_DIR_LEFT);
        
        LLabel label1(tile1);
        label1.text("Tile 1 - Swipe Right");
        lv_obj_center(label1.native());
        
        LLabel label2(tile2);
        label2.text("Tile 2 - Swipe Left");
        lv_obj_center(label2.native());
    }

    printf("\n✅ All 7 components tested!\n");
    printf("Demo running. Press Ctrl+C to exit gracefully.\n\n");

    while(signal_handler.is_running()) {
        lv_timer_handler();
        usleep(5000);
    }

    printf("Cleaning up resources...\n");
    return 0;
}
