//
// Created by Sprite on 2023/4/3.
//
#include "lv_touchSlider.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "testHal.h"
#include "lvgl.h"

#define DEMO_MAX 9

void swipeStartContainer2( lv_event_t * evt ){

    QVGLC_touchSliderTouchStart( 0, evt );
}

void swipeMoveContainer2( lv_event_t * evt ){

    QVGLC_touchSliderTouchMove( 0, evt );
}

void swipeEndContainer2( lv_event_t * evt ){

    QVGLC_touchSliderTouchUp( 0, evt );
}

void swipeStartContainer3( lv_event_t * evt ){

    QVGLC_touchSliderTouchStart( 1, evt );
}

void swipeMoveContainer3( lv_event_t * evt ){

    QVGLC_touchSliderTouchMove( 1, evt );
}

void swipeEndContainer3( lv_event_t * evt ){

    QVGLC_touchSliderTouchUp( 1, evt );
}

static char * pngIcons[DEMO_MAX] = {
    "S:/customer/res/60889_flip/images/mode_boiled.png",
    "S:/customer/res/60889_flip/images/mode_normal.png",
    "S:/customer/res/60889_flip/images/mode_warm.png",
    "S:/customer/res/60889_flip/images/mode_honey.png",
    "S:/customer/res/60889_flip/images/mode_tea.png",
    "S:/customer/res/60889_flip/images/mode_boiled.png",
    "S:/customer/res/60889_flip/images/mode_normal.png",
    "S:/customer/res/60889_flip/images/mode_warm.png",
    "S:/customer/res/60889_flip/images/mode_honey.png",
};

struct TouchSliderElement_t{

    lv_obj_t * icon;
    lv_obj_t * bg;
    lv_label_t * label;

};

void elementUpdatedCall( lv_obj_t * element, int idx, int selected, int fd ){

    if (idx==selected) {
        lv_obj_set_style_text_color(lv_obj_get_child(element,0), lv_color_hex(0xff3366ff), 0);
    }else{
        lv_obj_set_style_text_color(lv_obj_get_child(element,0), lv_color_hex(0xff555555), 0);
    }
}

void testImageSlider(){

    int max_image = 99;

    lv_obj_t * container2 = lv_obj_create(lv_scr_act());

    lv_obj_set_width(container2,800);
    lv_obj_set_height(container2,1280);
    lv_obj_set_style_bg_color(container2,lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_scrollbar_mode(container2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(container2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * image2 = lv_img_create(container2);
//    lv_img_set_src(image2,&bigicon);
    lv_img_set_src(image2,"S:/customer/res/60889_flip/images/background.png");

    lv_obj_set_style_pad_left(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_x(container2,0);

    lv_obj_t ** elements2 = (lv_obj_t**) malloc(max_image*sizeof (lv_obj_t*));

    for (int i = 0; i < max_image; i++) {

        elements2[i] = lv_obj_create( container2 );
        lv_obj_set_width(elements2[i],256);
        lv_obj_set_height(elements2[i],278);

        lv_obj_set_style_bg_color(elements2[i], lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(elements2[i], 0x0 ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(elements2[i], 0, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_x(elements2[i],260);
//        lv_obj_set_y(elements2[i],50);

//        lv_obj_t * txt = lv_label_create( elements2[i] );
//        lv_label_set_text_fmt(txt, "%d", i);
//        lv_obj_set_style_text_color( txt, lv_color_hex(0xff888888), 0 );

//        printf("PNG src %s\n",pngIcons[i]);

        lv_obj_t * img = lv_img_create( elements2[i] );
        lv_img_set_src( img, pngIcons[i%9] );
        lv_obj_set_scrollbar_mode(elements2[i], LV_SCROLLBAR_MODE_OFF);
        lv_obj_clear_flag(elements2[i], LV_OBJ_FLAG_SCROLLABLE);
        lv_img_set_antialias(img, false); // 移除抗锯齿

//        lv_obj_set_style_bg_img_src( elements2[i], &bigicon, LV_PART_MAIN | LV_STATE_DEFAULT );
//        lv_obj_set_style_bg_img_src( elements2[i], pngIcons[i], LV_PART_MAIN | LV_STATE_DEFAULT );
    }
    printf("Element created\n");

    QVGLC_TouchSliderConfig_t config2 = {
        .fx             = TOUCH_SLIDER_FX_MODE_CIRCLE,
        .direction      = TOUCH_SLIDER_DIRECTION_Y,
//        .direction      = TOUCH_SLIDER_DIRECTION_X,
//        .layout         = TOUCH_SLIDER_LAYOUT_LT,
        .layout         = TOUCH_SLIDER_LAYOUT_CENTER,
//        .layout         = TOUCH_SLIDER_LAYOUT_RB,
        .inertia        = 0.8f,
        .attenuation    = 2.5f,
        .canLoop        = true,
//    .canLoop        = false,
//        .useScale       = true,
//        .useScale       = false,
        .useOpacity     = true,
        .gapSize        = 256,
        .gapRatio       = 0.95f,
        .containerSize  = 1280,
        .objectWidth    = 256,
        .objectHeight   = 278,
        .index          = 5,
        .maxBrother     = 9,
        .radius         = 3200
    };

    int fd2 = QVGLC_touchSliderInit( elements2, max_image, &config2 );

    lv_obj_add_event_cb( container2, swipeStartContainer2, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb( container2, swipeMoveContainer2, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb( container2, swipeEndContainer2, LV_EVENT_RELEASED, NULL);

    QVGLC_touchSliderRegisterElementUpdate( fd2, elementUpdatedCall );


}
static lv_obj_t * container2;
void testLabelSlider(){

    int max_label = 99;

    container2 = lv_obj_create(lv_scr_act());

    lv_obj_set_width(container2,200);
    lv_obj_set_height(container2,480);
    lv_obj_set_style_bg_color(container2,lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_scrollbar_mode(container2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(container2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

//    lv_obj_t * image2 = lv_img_create(container2);
//    lv_img_set_src(image2,&bigicon);
//    lv_img_set_src(image2,"S:/customer/res/60889_flip/images/background.png");

    lv_obj_set_style_pad_left(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

//    lv_obj_set_y(container2,0);
    lv_obj_center(container2);



    lv_obj_t ** elements2 = (lv_obj_t**) malloc(max_label*sizeof (lv_obj_t*));

    for (int i = 0; i < max_label; i++) {

        elements2[i] = lv_obj_create( container2 );
        lv_obj_set_width(elements2[i],120);
        lv_obj_set_height(elements2[i],80);

        lv_obj_set_style_bg_color(elements2[i], lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(elements2[i], 0x0 ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(elements2[i], 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_clear_flag(container2, LV_OBJ_FLAG_SCROLLABLE);

//        lv_obj_set_x(elements2[i],260);
//        lv_obj_set_y(elements2[i],50);

        lv_obj_add_flag(elements2[i],LV_OBJ_FLAG_EVENT_BUBBLE);

        lv_obj_t * txt = lv_label_create( elements2[i] );
        lv_label_set_text_fmt(txt, "%d", i);
        lv_obj_set_style_text_color( txt, lv_color_hex(0xff555555), 0 );

    }
    printf("Element created\n");

    QVGLC_TouchSliderConfig_t config2 = {
            .direction      = TOUCH_SLIDER_DIRECTION_Y,
//            .direction      = TOUCH_SLIDER_DIRECTION_X,
//            .layout         = TOUCH_SLIDER_LAYOUT_LT,
            .layout         = TOUCH_SLIDER_LAYOUT_CENTER,
//            .layout         = TOUCH_SLIDER_LAYOUT_RB,
            .inertia        = 0.8f,
            .attenuation    = 2.5f,
            .canLoop        = true,
//        .canLoop        = false,
//            .useScale       = true,
        .useScale       = false,
            .useOpacity     = true,
            .gapSize        = 80,
            .containerSize  = 480,
            .objectWidth    = 120,
            .objectHeight   = 80,
            .index          = 0,
            .maxBrother     = 5,
    };

    int fd2 = QVGLC_touchSliderInit( elements2, max_label, &config2 );

    lv_obj_add_event_cb( container2, swipeStartContainer2, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb( container2, swipeMoveContainer2, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb( container2, swipeEndContainer2, LV_EVENT_RELEASED, NULL);

//    QVGLC_touchSliderRegisterElementUpdate( fd2, elementUpdatedCall );


}



void testLabelSliderUpper(){

    int max_label = 99;

    lv_obj_t * containerUpper = lv_obj_create(lv_scr_act());

    lv_obj_set_width(containerUpper,200);
    lv_obj_set_height(containerUpper,80);
    lv_obj_set_style_bg_color(containerUpper,lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_scrollbar_mode(containerUpper, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(containerUpper, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(containerUpper, LV_OBJ_FLAG_SCROLLABLE);

//    lv_obj_t * image2 = lv_img_create(containerUpper);
//    lv_img_set_src(image2,&bigicon);
//    lv_img_set_src(image2,"S:/customer/res/60889_flip/images/background.png");

    lv_obj_set_style_pad_left(containerUpper, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(containerUpper, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(containerUpper, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(containerUpper, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_center( containerUpper );

//    lv_obj_set_y(containerUpper,200);


    lv_obj_t ** elements2 = (lv_obj_t**) malloc(max_label*sizeof (lv_obj_t*));

    for (int i = 0; i < max_label; i++) {

        elements2[i] = lv_obj_create( containerUpper );
        lv_obj_set_width(elements2[i],120);
        lv_obj_set_height(elements2[i],80);

        lv_obj_set_style_bg_color(elements2[i], lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(elements2[i], 0x0 ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(elements2[i], 0, LV_PART_MAIN | LV_STATE_DEFAULT);

//        lv_obj_set_x(elements2[i],260);
//        lv_obj_set_y(elements2[i],50);

        lv_obj_add_flag(elements2[i],LV_OBJ_FLAG_EVENT_BUBBLE);

        lv_obj_t * txt = lv_label_create( elements2[i] );
        lv_label_set_text_fmt(txt, "%d", i);
        lv_obj_set_style_text_color( txt, lv_color_hex(0xffffffff), 0 );

    }
    printf("Element created\n");

    QVGLC_TouchSliderConfig_t config2 = {
            .direction      = TOUCH_SLIDER_DIRECTION_Y,
//            .direction      = TOUCH_SLIDER_DIRECTION_X,
//            .layout         = TOUCH_SLIDER_LAYOUT_LT,
            .layout         = TOUCH_SLIDER_LAYOUT_CENTER,
//            .layout         = TOUCH_SLIDER_LAYOUT_RB,
            .inertia        = 0.8f,
            .attenuation    = 2.5f,
            .canLoop        = true,
//        .canLoop        = false,
//            .useScale       = true,
            .useScale       = false,
            .useOpacity     = true,
            .gapSize        = 80,
            .containerSize  = 80,
            .objectWidth    = 120,
            .objectHeight   = 80,
            .index          = 0,
            .maxBrother     = 3,
    };

    int fd2 = QVGLC_touchSliderInit( elements2, max_label, &config2 );

    lv_obj_add_event_cb( container2, swipeStartContainer3, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb( container2, swipeMoveContainer3, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb( container2, swipeEndContainer3, LV_EVENT_RELEASED, NULL);

//    QVGLC_touchSliderRegisterElementUpdate( fd2, elementUpdatedCall );


}



void testRadiusLabelSlider(){

    int max_label = 299;

    lv_obj_t * container2 = lv_obj_create(lv_scr_act());

    lv_obj_set_width(container2,800);
    lv_obj_set_height(container2,480);
    lv_obj_set_style_bg_color(container2,lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_scrollbar_mode(container2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(container2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * image2 = lv_img_create(container2);
//    lv_img_set_src(image2,&bigicon);
    lv_img_set_src(image2,"S:/customer/res/60889_flip/images/background.png");

    lv_obj_set_style_pad_left(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_y(container2,0);



    lv_obj_t ** elements2 = (lv_obj_t**) malloc(max_label*sizeof (lv_obj_t*));

    for (int i = 0; i < max_label; i++) {

        elements2[i] = lv_obj_create( container2 );
        lv_obj_set_width(elements2[i],120);
        lv_obj_set_height(elements2[i],60);

        lv_obj_set_style_bg_color(elements2[i], lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(elements2[i], 0x0 ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(elements2[i], 0, LV_PART_MAIN | LV_STATE_DEFAULT);

//        lv_obj_set_x(elements2[i],260);
        lv_obj_set_y(elements2[i],50);

        lv_obj_t * txt = lv_label_create( elements2[i] );
        lv_label_set_text_fmt(txt, "%d", i);
        lv_obj_set_style_text_color( txt, lv_color_hex(0xff555555), 0 );

    }
    printf("Element created\n");

    QVGLC_TouchSliderConfig_t config2 = {
//            .fx             = TOUCH_SLIDER_FX_MODE_CIRCLE,
            .direction      = TOUCH_SLIDER_DIRECTION_Y,
//            .direction      = TOUCH_SLIDER_DIRECTION_X,
//            .layout         = TOUCH_SLIDER_LAYOUT_LT,
            .layout         = TOUCH_SLIDER_LAYOUT_CENTER,
//            .layout         = TOUCH_SLIDER_LAYOUT_RB,
            .inertia        = 0.8f,
            .attenuation    = 2.5f,
            .canLoop        = true,
//        .canLoop        = false,
//            .useScale       = true,
            .useScale       = false,
            .useOpacity     = true,
            .gapSize        = 60,
//            .gapRatio       = 0.8f,
            .containerSize  = 480,
            .objectWidth    = 120,
            .objectHeight   = 60,
            .index          = 0,
            .maxBrother     = 2,
            .springEdgeRatio= 0.8f,
//            .radius         = 300
    };

    int fd2 = QVGLC_touchSliderInit( elements2, max_label, &config2 );

    lv_obj_add_event_cb( container2, swipeStartContainer2, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb( container2, swipeMoveContainer2, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb( container2, swipeEndContainer2, LV_EVENT_RELEASED, NULL);

    QVGLC_touchSliderRegisterElementUpdate( fd2, elementUpdatedCall );


}


void testVerticalRadiusLabelSlider(){

    int max_label = 299;

    lv_obj_t * container2 = lv_obj_create(lv_scr_act());

    lv_obj_set_width(container2,800);
    lv_obj_set_height(container2,1280);
    lv_obj_set_style_bg_color(container2,lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_scrollbar_mode(container2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(container2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * image2 = lv_img_create(container2);
//    lv_img_set_src(image2,&bigicon);
    lv_img_set_src(image2,"S:/customer/res/60889_flip/images/background.png");

    lv_obj_set_style_pad_left(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_y(container2,0);



    lv_obj_t ** elements2 = (lv_obj_t**) malloc(max_label*sizeof (lv_obj_t*));

    for (int i = 0; i < max_label; i++) {

        elements2[i] = lv_obj_create( container2 );
        lv_obj_set_width(elements2[i],120);
        lv_obj_set_height(elements2[i],120);

        lv_obj_set_style_bg_color(elements2[i], lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(elements2[i], 0x0 ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(elements2[i], 0, LV_PART_MAIN | LV_STATE_DEFAULT);

//        lv_obj_set_x(elements2[i],260);
        lv_obj_set_y(elements2[i],50);

        lv_obj_t * txt = lv_label_create( elements2[i] );
        lv_label_set_text_fmt(txt, "%d", i);
        lv_obj_set_style_text_color( txt, lv_color_hex(0xff555555), 0 );

    }
    printf("Element created\n");

    QVGLC_TouchSliderConfig_t config2 = {
//            .fx             = TOUCH_SLIDER_FX_MODE_CIRCLE,
//            .direction      = TOUCH_SLIDER_DIRECTION_Y,
            .direction      = TOUCH_SLIDER_DIRECTION_X,
//            .layout         = TOUCH_SLIDER_LAYOUT_LT,
            .layout         = TOUCH_SLIDER_LAYOUT_CENTER,
//            .layout         = TOUCH_SLIDER_LAYOUT_RB,
            .inertia        = 0.8f,
            .attenuation    = 2.5f,
            .canLoop        = true,
//        .canLoop        = false,
//            .useScale       = true,
            .useScale       = false,
            .useOpacity     = true,
            .gapSize        = 80,
//            .gapRatio       = 0.8,
            .containerSize  = 800,
            .objectWidth    = 80,
            .objectHeight   = 80,
            .index          = 0,
            .maxBrother     = 9,
            .springEdgeRatio = 0.75f,
            .radius         = 1600
    };

    int fd2 = QVGLC_touchSliderInit( elements2, max_label, &config2 );

    lv_obj_add_event_cb( container2, swipeStartContainer2, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb( container2, swipeMoveContainer2, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb( container2, swipeEndContainer2, LV_EVENT_RELEASED, NULL);

    QVGLC_touchSliderRegisterElementUpdate( fd2, elementUpdatedCall );


}


int main(){

    LV_IMG_DECLARE(bigicon);

    lv_init();

    testHalInit();

    printf("START TOUCH SLIDER DEMO\n");

    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);

//    testImageSlider();

    testLabelSlider();
    testLabelSliderUpper();

//    testRadiusLabelSlider();
//    testVerticalRadiusLabelSlider();

    printf("touchSliderInit\n");

    testHalLoop();
}
