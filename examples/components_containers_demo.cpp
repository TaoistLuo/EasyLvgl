#include "lvgl/lvgl.h"
#include "lvgl++/lvgl_widgets.hpp"
#include "lvgl++/containers/vstack.hpp"
#include "lvgl++/core/signal_handler.hpp"
#include <unistd.h>
#include <cstdio>

using namespace lvgl;

int main(void) {
    lv_init();
    lv_display_t* disp = lv_sdl_window_create(800, 600); (void)disp;
    lv_sdl_mouse_create();

    SignalHandler signal_handler;

    printf("=== LVGL++ Containers Demo ===\n");
    printf("Press Ctrl+C to exit gracefully.\n\n");

    LLabel title;
    title.text("Container Layouts")
         .pos(10, 10)
         .color(0x2196F3);

    LVStack vstack;
    vstack.pos(10, 50)
          .size(200, 400);

    LLabel label1;
    label1.text("VStack Layout");
    lv_obj_set_parent(label1.native(), vstack.native());

    LButton btn1;
    btn1.text("Button 1").size(180, 40);
    lv_obj_set_parent(btn1.native(), vstack.native());

    LButton btn2;
    btn2.text("Button 2").size(180, 40);
    lv_obj_set_parent(btn2.native(), vstack.native());

    LButton btn3;
    btn3.text("Button 3").size(180, 40);
    lv_obj_set_parent(btn3.native(), vstack.native());

    LSlider slider;
    slider.range(0, 100).value(50).size(180, 10);
    lv_obj_set_parent(slider.native(), vstack.native());

    LBar bar;
    bar.range(0, 100).value(75).size(180, 20);
    lv_obj_set_parent(bar.native(), vstack.native());

    LList list;
    list.pos(230, 50)
        .size(250, 400);
    
    list.add_text("Section 1");
    list.add_button(LV_SYMBOL_FILE, "Document 1");
    list.add_button(LV_SYMBOL_FILE, "Document 2");
    list.add_button(LV_SYMBOL_DIRECTORY, "Folder 1");
    
    list.add_text("Section 2");
    list.add_button(LV_SYMBOL_IMAGE, "Image 1");
    list.add_button(LV_SYMBOL_VIDEO, "Video 1");
    list.add_button(LV_SYMBOL_AUDIO, "Audio 1");
    
    list.add_text("Section 3");
    list.add_button(LV_SYMBOL_OK, "Completed");
    list.add_button(LV_SYMBOL_CLOSE, "Cancelled");

    LTabView tabview;
    tabview.pos(500, 50)
           .size(300, 400);
    
    auto tab1 = tabview.add_tab("Tab 1");
    auto tab2 = tabview.add_tab("Tab 2");
    auto tab3 = tabview.add_tab("Tab 3");
    
    LLabel tab1_label;
    tab1_label.text("Content of Tab 1");
    lv_obj_set_parent(tab1_label.native(), tab1);
    lv_obj_center(tab1_label.native());
    
    LLabel tab2_label;
    tab2_label.text("Content of Tab 2");
    lv_obj_set_parent(tab2_label.native(), tab2);
    lv_obj_center(tab2_label.native());
    
    LLabel tab3_label;
    tab3_label.text("Content of Tab 3");
    lv_obj_set_parent(tab3_label.native(), tab3);
    lv_obj_center(tab3_label.native());

    printf("\nDemo running. Press Ctrl+C to exit gracefully.\n");

    while(signal_handler.is_running()) {
        lv_timer_handler();
        usleep(5000);
    }

    printf("\nCleaning up resources...\n");
    return 0;
}
