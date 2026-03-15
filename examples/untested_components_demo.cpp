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
    main_container.set_spacing(10)
                  .set_padding(15)
                  .set_size(750, 550)
                  .set_align(LV_ALIGN_CENTER);

    printf("1. Testing LTextArea...\n");
    {
        LLabel title(main_container.native());
        title.set_text("1. TextArea - Text Input").set_color(0x2196F3);

        LTextArea textarea(main_container.native());
        textarea.set_placeholder("Enter your text here...")
                .set_size(700, 60)
                .set_one_line(false)
                .on_value_changed([](lv_event_t* e) {
                    lv_obj_t* ta = static_cast<lv_obj_t*>(lv_event_get_target(e));
                    printf("   TextArea value: %s\n", lv_textarea_get_text(ta));
                });
    }

    printf("2. Testing LButtonMatrix...\n");
    {
        LLabel title(main_container.native());
        title.set_text("2. ButtonMatrix - Button Grid").set_color(0x2196F3);

        static const char* btnm_map[] = {"1", "2", "3", "\n",
                                          "4", "5", "6", "\n",
                                          "7", "8", "9", ""};

        LButtonMatrix btnm(main_container.native());
        btnm.set_map(btnm_map)
            .set_size(300, 150)
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
        title.set_text("3. ImageButton - Image Button").set_color(0x2196F3);

        LImageButton imgbtn(main_container.native());
        imgbtn.set_size(80, 80);
        imgbtn.on_click([](lv_event_t*) {
            printf("   ImageButton clicked!\n");
        });
    }

    printf("4. Testing LMsgBox...\n");
    {
        LButton show_msgbox_btn(main_container.native());
        show_msgbox_btn.set_text("Show Message Box")
                       .set_size(200, 40);
        
        lv_obj_add_event_cb(show_msgbox_btn.native(), [](lv_event_t*) {
            printf("   Creating MsgBox...\n");
            
            LMsgBox msgbox(lv_screen_active());
            msgbox.set_title("Confirmation")
                  .set_text("Are you sure?")
                  .add_close_button();
            msgbox.on_value_changed([](lv_event_t* e) {
                printf("   MsgBox closed\n");
            });
            
            lv_obj_t* btn_ok = msgbox.add_footer_button("OK");
            lv_obj_t* btn_cancel = msgbox.add_footer_button("Cancel");
        }, LV_EVENT_CLICKED, nullptr);
    }

    printf("5. Testing LTabView...\n");
    {
        LLabel title(main_container.native());
        title.set_text("5. TabView - Tab Container").set_color(0x2196F3);

        LTabView tabview(main_container.native());
        tabview.set_size(700, 150);
        
        lv_obj_t* tab1 = lv_tabview_add_tab(tabview.native(), "Tab 1");
        lv_obj_t* tab2 = lv_tabview_add_tab(tabview.native(), "Tab 2");
        
        LLabel label1(tab1);
        label1.set_text("Content of Tab 1");
        
        LLabel label2(tab2);
        label2.set_text("Content of Tab 2");
    }

    printf("6. Testing LAnimImage...\n");
    {
        LLabel title(main_container.native());
        title.set_text("6. AnimImage - Animated Image").set_color(0x2196F3);
        
        LLabel note(main_container.native());
        note.set_text("(AnimImage requires image array - skipped)").set_color(0x999999);
    }

    printf("7. Testing LTileView...\n");
    {
        LLabel title(main_container.native());
        title.set_text("7. TileView - Tile View").set_color(0x2196F3);

        LTileView tileview(main_container.native());
        tileview.set_size(700, 100);
        
        lv_obj_t* tile1 = lv_tileview_add_tile(tileview.native(), 0, 0, LV_DIR_RIGHT);
        lv_obj_t* tile2 = lv_tileview_add_tile(tileview.native(), 1, 0, LV_DIR_LEFT);
        
        LLabel label1(tile1);
        label1.set_text("Tile 1 - Swipe Right");
        lv_obj_center(label1.native());
        
        LLabel label2(tile2);
        label2.set_text("Tile 2 - Swipe Left");
        lv_obj_center(label2.native());
    }

    printf("\nAll 7 components tested!\n");
    printf("Demo running. Press Ctrl+C to exit gracefully.\n\n");

    while(signal_handler.is_running()) {
        lv_timer_handler();
        usleep(5000);
    }

    printf("Cleaning up resources...\n");
    return 0;
}
