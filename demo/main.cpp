//
// Created by mac on 2023/3/5.
//

#include "QuickVGL.hpp"

#include "cstdio"
#include "testimg.h"
#include "signal.h"
#include "main.hpp"

#include "lv_touchSlider.h"
#include "indev/evdev.h"

typedef enum {

    HomePageID,
    CurrentTempID,

    BrightnessSettingPageID,
    BrightnessLeftPanelID,

    LightOnTitleText,

    IconOK_ID
} AppID ;

QString * labelText;
QLabel * qLabel;
QView  * qView1;

/* Callback for Timer pressed */
static void TimerCb(lv_timer_t *timer) {

    std::string strings[5] = {
            "Apple",
            "panda",
            "snake",
            "monkey",
            "tiger"
    };

    static int pressed = 0;
    pressed = (pressed+1) % 4;

    *labelText = strings[pressed];

    if ( pressed % 3 == 0 )
        qLabel->hide();
    else
        qLabel->show();

    if ( pressed % 2 == 0 )
        qView1->hide();
    else
        qView1->show();

}

void test_demoUI(){

    auto * body = new QView( { 450, 300 }, {150,50} );

    qView1 = new QView( { 150,150 }, {0,0} );
    auto * qView2 = (new QView( { 250,150 }, {10,0} ))->bg({255,255,66,128})->hash(333);

    body->insert( qView1 );
    body->insert( qView2 );

    qView2->pos({ 120, 150 });

    labelText = new QString("Hello QLabel");
    qLabel = new QLabel( labelText );

    qView2->insert( qLabel );

    *labelText = " Test Qstring binding ";


    auto qLabel2 = new QLabel( labelText );

    body->insert(qLabel2);

//    initSecondPage();
//    QPage::getByID( HomePageID )->hide();
//    qqLabel.append("b").append("c");
//    QString::getByID( CurrentTempID )->set( "27.6" );
//    QInt::getByID( CurrentTempID );

    printf("%s\n", QString::query( CurrentTempID )->set( "27.6" )->value().c_str() );

    /* Timer */
    auto *timer = new QTimer();                // or timer = new LvTimer();
    timer->setPeriod(1000)->
            setCb(TimerCb)->
            ready();



}




/* Callback for Timer pressed */
static void test_61569UI_TimerCb(lv_timer_t *timer) {

    static int pressed = 0, pos = 0;
    static char left = 0,top = 0;
    pressed = (pressed+1) % 50;
    left+=2;
    top-=1;

    if ( pressed > 25 ) {
        qview(BrightnessSettingPageID)->bg(qimgres("PowerOnIcon"));
        qlabel(LightOnTitleText)->text( new QString("同时点亮 同时熄灭"));
    }
    else {
//        qview(BrightnessSettingPageID)->bg(qimgres("alien"));
        qlabel(LightOnTitleText)->text( new QString("同时点亮 | 同时熄灭"));
    }
    qview(BrightnessLeftPanelID)->pos(left, top);

//    printf("id %d  pressed %d \n",pos++, pressed);
    static int k = 0;

    if ( pressed < 25 ){
//        qimage("alien")->setSource(qimgres("alien") )->pos(top, left);
    }else{
//        qimage("alien")->setSource(qimgres("IconOkay") )->pos(top, left);
    }

//        printf("K%d  top %d, left %d\n",k++, top, left);

    // FIXME
    // 通过qimage获取到的image在特定情况下会出现无法拿到data的问题。暂不清楚原因
}


void test_61569UI_addToScreen(){

    qview( BrightnessSettingPageID )
        ->insert(
            qview(BrightnessLeftPanelID)
//            ->insert( qimage("ConfirmDelColor")->pos(50,150)->show() )
//            ->bg(qimgres("IconOkay") )
    );

    qview(BrightnessSettingPageID)
//        ->insert(
//            qimage("alien")->size({320, 240})->pos(300,50)->show())
        ->insert(
            qimage("PowerOffIcon")->show());
}

void test_61569UI(){

    printf("Hello QVGL test_61569UI\n");

//    ImageSet( "ConfirmDelColor",    &testimgdst );
//    ImageSet( "IconOkay",           &testimgdst2 );
//    ImageSet( "alien",              "S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/alien.sjpg" );
    ImageSet( "PowerOnIcon",        "S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/btn-power-press.png" );
    ImageSet( "PowerOffIcon",       "S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/btn-power-normal.png" );
    ImageSet( "brightness-always-on-on", "S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/brightness-always-on-on.png");
    ImageSet( "brightness-always-on-off", "S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/brightness-always-on-off.png");

    FontSet( "CNFont", "/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/fonts/fzhtjw.ttf" );
    FontSet( "ENFont", "/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/fonts/arial.ttf" );

//    FontSet( "CNFont", "/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/fonts/fzhtjw.ttf" );
//    FontSet( "ENFont", "/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/fonts/arial.ttf" );

    QV(800,480,0,0)
    ->hash(BrightnessSettingPageID)
    ->clean()->bg(0x0C0D0EFF);

    QV(380,245,15,110)
    ->hash(BrightnessLeftPanelID)
    ->clean()->bg(0x212328FF,0xADB0BDFF)->radius(14);

    qview(BrightnessSettingPageID)
        ->insert(
            qview(BrightnessLeftPanelID)
                ->insert( QV(40,40,20,20)->clean()->bg(0x0C0D0EFF)->radius(20)->bg(qimgres("PowerOffIcon"), qimgres("PowerOnIcon")))
                ->insert( QL("常亮模式")->font(qfont("CNFont"))->pos(60,20) )
                ->insert( QL("同时点亮 同时熄灭")->font(qfont("CNFont"))->pos(60,60)->hash(LightOnTitleText) )
                ->insert( QI("brightness-always-on-off")->pos(155,90)->show() )
            )
        ;

    test_61569UI_addToScreen();

    /* Timer */
    auto *timer = new QTimer();
    timer->setPeriod(15)->setCb(test_61569UI_TimerCb)->ready();

}

void swipeStartContainer2( lv_event_t * evt ){

    QVGLC_touchSliderTouchStart( 0, evt );
}

void swipeMoveContainer2( lv_event_t * evt ){

    QVGLC_touchSliderTouchMove( 0, evt );
}

void swipeEndContainer2( lv_event_t * evt ){

    QVGLC_touchSliderTouchUp( 0, evt );
}


const std::string pngIcons[9] = {
    "S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/60889_flip/images/mode_boiled.png",
    "S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/60889_flip/images/mode_normal.png",
    "S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/60889_flip/images/mode_warm.png",
    "S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/60889_flip/images/mode_honey.png",
    "S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/60889_flip/images/mode_tea.png",
    "S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/60889_flip/images/mode_boiled.png",
    "S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/60889_flip/images/mode_normal.png",
    "S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/60889_flip/images/mode_warm.png",
    "S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/60889_flip/images/mode_honey.png",
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
    lv_img_set_src(image2,"S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/60889_flip/images/background.png");

    lv_obj_set_style_pad_left(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_x(container2,0);

    auto ** elements2 = (lv_obj_t**) malloc(max_image*sizeof (lv_obj_t*));

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
        lv_img_set_src( img, pngIcons[i%9].c_str() );
        lv_obj_set_scrollbar_mode(elements2[i], LV_SCROLLBAR_MODE_OFF);
        lv_obj_clear_flag(elements2[i], LV_OBJ_FLAG_SCROLLABLE);
        lv_img_set_antialias(img, false); // 移除抗锯齿

//        lv_obj_set_style_bg_img_src( elements2[i], &bigicon, LV_PART_MAIN | LV_STATE_DEFAULT );
//        lv_obj_set_style_bg_img_src( elements2[i], pngIcons[i], LV_PART_MAIN | LV_STATE_DEFAULT );
    }
    printf("Element created\n");

    QVGLC_TouchSliderConfig_t config2 = {
//            .fx             = TOUCH_SLIDER_FX_MODE_CIRCLE,
//            .direction      = TOUCH_SLIDER_DIRECTION_Y,
        .direction      = TOUCH_SLIDER_DIRECTION_X,
        .layout         = TOUCH_SLIDER_LAYOUT_LT,
//            .layout         = TOUCH_SLIDER_LAYOUT_CENTER,
//        .layout         = TOUCH_SLIDER_LAYOUT_RB,
            .containerSize  = 1280,
            .objectWidth    = 256,
            .objectHeight   = 278,
            .gapSize        = 128,
            .gapRatio       = 0.95,
//            .canLoop        = true,
    .canLoop        = false,
//        .useScale       = true,
//        .useScale       = false,
            .useOpacity     = true,
            .index          = 0,
            .inertia        = 0.8,
            .attenuation    = 2.5,
            .maxBrother     = 6,
//            .radius         = 250
    };

    int fd2 = QVGLC_touchSliderInit( elements2, max_image, &config2 );

    lv_obj_add_event_cb( container2, swipeStartContainer2, LV_EVENT_PRESSED, nullptr);
    lv_obj_add_event_cb( container2, swipeMoveContainer2, LV_EVENT_PRESSING, nullptr);
    lv_obj_add_event_cb( container2, swipeEndContainer2, LV_EVENT_RELEASED, nullptr);

    QVGLC_touchSliderRegisterElementUpdate( fd2, elementUpdatedCall );


}

void testLabelSlider(){

    int max_label = 299;

    lv_obj_t * container2 = lv_obj_create(lv_scr_act());

    lv_obj_set_width(container2,800);
    lv_obj_set_height(container2,480);
    lv_obj_set_style_bg_color(container2,lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_scrollbar_mode(container2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(container2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * image2 = lv_img_create(container2);
//    lv_img_set_src(image2,&bigicon);
    lv_img_set_src(image2,"S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/60889_flip/images/background.png");

    lv_obj_set_style_pad_left(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_y(container2,0);

    auto ** elements2 = (lv_obj_t**) malloc(max_label*sizeof (lv_obj_t*));

    for (int i = 0; i < max_label; i++) {

        elements2[i] = lv_obj_create( container2 );
        lv_obj_set_width(elements2[i],180);
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
            .direction      = TOUCH_SLIDER_DIRECTION_Y,
//            .direction      = TOUCH_SLIDER_DIRECTION_X,
//            .layout         = TOUCH_SLIDER_LAYOUT_LT,
            .layout         = TOUCH_SLIDER_LAYOUT_CENTER,
//            .layout         = TOUCH_SLIDER_LAYOUT_RB,
            .containerSize  = 480,
            .objectWidth    = 100,
            .objectHeight   = 60,
            .gapSize        = 60,
            .canLoop        = true,
//        .canLoop        = false,
//            .useScale       = true,
            .useScale       = false,
            .useOpacity     = true,
            .index          = 0,
            .inertia        = 0.8,
            .attenuation    = 2.5,
            .maxBrother     = 3,
    };

    int fd2 = QVGLC_touchSliderInit( elements2, max_label, &config2 );

    lv_obj_add_event_cb( container2, swipeStartContainer2, LV_EVENT_PRESSED, nullptr);
    lv_obj_add_event_cb( container2, swipeMoveContainer2, LV_EVENT_PRESSING, nullptr);
    lv_obj_add_event_cb( container2, swipeEndContainer2, LV_EVENT_RELEASED, nullptr);

    QVGLC_touchSliderRegisterElementUpdate( fd2, elementUpdatedCall );


}



void testRadiusLabelSlider(){

    int max_label = 299;

    auto * sliderContainer = QV(800,480,0,0)->clean()->noScroll()->bg({0,0,0,0xff})->bg(qimgres("DarkBg"));

    for (int i = 0; i < max_label; i++)
        sliderContainer->insert(
            QV( 120,120, 0,50 )->clean()->noScroll()->bg((uint32_t)0x0)->insert( QL(i)->textColor( 0xff555555 ) )
        );

    auto qTouchSliderController = new QTouchSlider( sliderContainer );
        qTouchSliderController
        ->setSize( 480, 50 )->setElementSize( 120, 50)
//        ->setCircleMode(2400)
        ->visibleSiblings(15)
        ->indexTo(0)
        ->dirY()
        ->layoutCenter()
        ->useOpacity()
        ->noScale()
        ->loop()
        ->initSlider();

}



void testRadiusLabelSliderWhite(){

    int max_label = 299;

    auto * sliderContainer = QV(800,50,0,215)->clean()->noScroll()->bg({0,0,0,0xff})->bg(qimgres("DarkBg"));

    for (int i = 0; i < max_label; i++)
        sliderContainer->insert(
                QV( 120,120, 0,50 )->clean()->noScroll()->bg((uint32_t)0x0)->insert( QL(i)->textColor( 0xffffffff ) )
        );

    auto qTouchSliderController = new QTouchSlider( sliderContainer );
    qTouchSliderController
            ->setSize( 480, 50 )->setElementSize( 120, 50)
//            ->setCircleMode(2400)
            ->visibleSiblings(15)
            ->indexTo(0)
            ->dirY()
            ->layoutCenter()
            ->useOpacity()
            ->noScale()
            ->loop()
            ->initSlider();

}

void testVerticalRadiusLabelSlider(){

    int max_label = 299;

    auto * sliderContainer = QV(800,480,0,0)->clean()->noScroll()->bg({0,0,0,0xff})->bg(qimgres("DarkBg"));

    for (int i = 0; i < max_label; i++)
        sliderContainer->insert(
            QV( 120,120, 0,50 )->clean()->noScroll()->bg((uint32_t)0xff334455)->insert( QL(i)->textColor( 0xff555555 ) )
        );

    auto qTouchSliderController = new QTouchSlider( sliderContainer );
        qTouchSliderController
        ->setSize( 800, 80 )->setElementSize( 80,80 )
        ->setCircleMode(1600)
        ->visibleSiblings(15)
        ->indexTo(0)->dirX()
        ->layoutCenter()
        ->useOpacity()
        ->noScale()
        ->loop()
        ->initSlider();

}



static void mask_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    static int16_t mask_top_id = -1;
    static int16_t mask_bottom_id = -1;

    if(code == LV_EVENT_COVER_CHECK) {
        lv_event_set_cover_res(e, LV_COVER_RES_MASKED);

    }
    else if(code == LV_EVENT_DRAW_MAIN_BEGIN) {
        /* add mask */
        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
        lv_coord_t font_h = lv_font_get_line_height(font);

        lv_area_t roller_coords;
        lv_obj_get_coords(obj, &roller_coords);

        lv_area_t rect_area;
        rect_area.x1 = roller_coords.x1;
        rect_area.x2 = roller_coords.x2;
        rect_area.y1 = roller_coords.y1;
        rect_area.y2 = roller_coords.y1 + (lv_obj_get_height(obj) - font_h - line_space) / 2;

        auto * fade_mask_top = static_cast<lv_draw_mask_fade_param_t *>(lv_malloc(sizeof(lv_draw_mask_fade_param_t)));
        lv_draw_mask_fade_init(fade_mask_top, &rect_area, LV_OPA_TRANSP, rect_area.y1, LV_OPA_COVER, rect_area.y2);
        mask_top_id = lv_draw_mask_add(fade_mask_top, NULL);

        rect_area.y1 = rect_area.y2 + font_h + line_space - 1;
        rect_area.y2 = roller_coords.y2;

        auto * fade_mask_bottom = static_cast<lv_draw_mask_fade_param_t *>(lv_malloc(sizeof(lv_draw_mask_fade_param_t)));
        lv_draw_mask_fade_init(fade_mask_bottom, &rect_area, LV_OPA_COVER, rect_area.y1, LV_OPA_TRANSP, rect_area.y2);
        mask_bottom_id = lv_draw_mask_add(fade_mask_bottom, NULL);

    }
    else if(code == LV_EVENT_DRAW_POST_END) {
        auto * fade_mask_top = static_cast<lv_draw_mask_fade_param_t *>(lv_draw_mask_remove_id(
                mask_top_id));
        auto * fade_mask_bottom = static_cast<lv_draw_mask_fade_param_t *>(lv_draw_mask_remove_id(
                mask_bottom_id));
        lv_draw_mask_free_param(fade_mask_top);
        lv_draw_mask_free_param(fade_mask_bottom);
        lv_free(fade_mask_top);
        lv_free(fade_mask_bottom);
        mask_top_id = -1;
        mask_bottom_id = -1;
    }
}

/**
 * Add a fade mask to roller.
 */
void test_lvgl(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_color_black());
    lv_style_set_text_color(&style, lv_color_white());
    lv_style_set_border_width(&style, 0);
    lv_style_set_pad_all(&style, 0);
    lv_obj_add_style(lv_scr_act(), &style, 0);

    lv_obj_t * roller1 = lv_roller_create(lv_scr_act());
    lv_obj_add_style(roller1, &style, 0);
    lv_obj_set_style_bg_opa(roller1, LV_OPA_TRANSP, LV_PART_SELECTED);

#if LV_FONT_MONTSERRAT_22
    lv_obj_set_style_text_font(roller1, &lv_font_montserrat_22, LV_PART_SELECTED);
#endif

    lv_roller_set_options(roller1,
                          "January\n"
                          "February\n"
                          "March\n"
                          "April\n"
                          "May\n"
                          "June\n"
                          "July\n"
                          "August\n"
                          "September\n"
                          "October\n"
                          "November\n"
                          "December",
                          LV_ROLLER_MODE_NORMAL);

    lv_obj_center(roller1);
    lv_roller_set_visible_row_count(roller1, 3);
    lv_obj_add_event_cb(roller1, mask_event_cb, LV_EVENT_ALL, nullptr);
}

/**
 * Opacity and Transformations
 */
void lv_example_style_15(void)
{
    lv_obj_t * btn;
    lv_obj_t * label;

    /*Normal button*/
    btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 100, 40);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, -70);

    label = lv_label_create(btn);
    lv_label_set_text(label, "Normal");
    lv_obj_center(label);

    /*Set opacity
     *The button and the label is rendered to a layer first and that layer is blended*/
    btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 100, 40);
    lv_obj_set_style_opa(btn, LV_OPA_50, 0);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);

    label = lv_label_create(btn);
    lv_label_set_text(label, "Opa:50%");
    lv_obj_center(label);

    /*Set transformations
     *The button and the label is rendered to a layer first and that layer is transformed*/
    btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 100, 40);
    lv_obj_set_style_transform_angle(btn, 150, 0);        /*15 deg*/
    lv_obj_set_style_transform_zoom(btn, 128, 0);   /*1.25x*/
    lv_obj_set_style_transform_pivot_x(btn, 50, 0);
    lv_obj_set_style_transform_pivot_y(btn, 20, 0);
    lv_obj_set_style_opa(btn, LV_OPA_50, 0);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 70);

    label = lv_label_create(btn);
    lv_label_set_text(label, "Transf.");
    lv_obj_center(label);
}


/**
 * Creating a transition
 */
void lv_example_style_transition()
{
    static const lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, LV_STYLE_BORDER_COLOR, LV_STYLE_BORDER_WIDTH, 0};

    /* A default transition
     * Make it fast (100ms) and start with some delay (200 ms)*/
    static lv_style_transition_dsc_t trans_def;
    lv_style_transition_dsc_init(&trans_def, props, lv_anim_path_linear, 100, 200, NULL);

    /* A special transition when going to pressed state
     * Make it slow (500 ms) but start  without delay*/
    static lv_style_transition_dsc_t trans_pr;
    lv_style_transition_dsc_init(&trans_pr, props, lv_anim_path_linear, 500, 0, NULL);

    static lv_style_t style_def;
    lv_style_init(&style_def);
    lv_style_set_transition(&style_def, &trans_def);

    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_bg_color(&style_pr, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_border_width(&style_pr, 6);
    lv_style_set_border_color(&style_pr, lv_palette_darken(LV_PALETTE_RED, 3));
    lv_style_set_transition(&style_pr, &trans_pr);

    /*Create an object with the new style_pr*/
    lv_obj_t * obj = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj, &style_def, 0);
    lv_obj_add_style(obj, &style_pr, LV_STATE_PRESSED);

    lv_obj_center(obj);
}




/**
 * Creating a transition
 */
void lv_example_style_transitionFont()
{
    static const lv_style_prop_t props[] = {LV_STYLE_TRANSFORM_ZOOM,LV_STYLE_BG_OPA, 0};

    /* A default transition
     * Make it fast (100ms) and start with some delay (200 ms)*/
    static lv_style_transition_dsc_t trans_def;
    lv_style_transition_dsc_init(&trans_def, props, lv_anim_path_linear, 100, 200, nullptr);

    /* A special transition when going to pressed state
     * Make it slow (500 ms) but start  without delay*/
    static lv_style_transition_dsc_t trans_pr;
    lv_style_transition_dsc_init(&trans_pr, props, lv_anim_path_linear, 500, 0, nullptr);

    static lv_style_t style_def;
    lv_style_init(&style_def);
    lv_style_set_transition(&style_def, &trans_def);

    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_bg_color(&style_pr, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_transform_zoom( &style_pr, 128);
    lv_style_set_border_width(&style_pr, 6);
    lv_style_set_bg_opa( &style_pr, 0);
    lv_style_set_border_color(&style_pr, lv_palette_darken(LV_PALETTE_RED, 3));
    lv_style_set_transition(&style_pr, &trans_pr);

    /*Create an object with the new style_pr*/
    lv_obj_t * obj = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj, &style_def, 0);
    lv_obj_add_style(obj, &style_pr, LV_STATE_PRESSED);

    lv_obj_center(obj);
}




static lv_obj_t * chart1;
static lv_chart_series_t * ser1;
static lv_chart_series_t * ser2;

static void draw_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);

    /*Add the faded area before the lines are drawn*/
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(dsc->part == LV_PART_ITEMS) {
        if(!dsc->p1 || !dsc->p2) return;

        /*Add a line mask that keeps the area below the line*/
        lv_draw_mask_line_param_t line_mask_param;
        lv_draw_mask_line_points_init(&line_mask_param, dsc->p1->x, dsc->p1->y, dsc->p2->x, dsc->p2->y,
                                      LV_DRAW_MASK_LINE_SIDE_BOTTOM);
        int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);

        /*Add a fade effect: transparent bottom covering top*/
        lv_coord_t h = lv_obj_get_height(obj);
        lv_draw_mask_fade_param_t fade_mask_param;
        lv_draw_mask_fade_init(&fade_mask_param, &obj->coords, LV_OPA_COVER, obj->coords.y1 + h / 8, LV_OPA_TRANSP,
                               obj->coords.y2);
        int16_t fade_mask_id = lv_draw_mask_add(&fade_mask_param, NULL);

        /*Draw a rectangle that will be affected by the mask*/
        lv_draw_rect_dsc_t draw_rect_dsc;
        lv_draw_rect_dsc_init(&draw_rect_dsc);
        draw_rect_dsc.bg_opa = LV_OPA_40;
        draw_rect_dsc.bg_color = dsc->line_dsc->color;

        lv_area_t a;
        a.x1 = dsc->p1->x;
        a.x2 = dsc->p2->x - 1;
        a.y1 = LV_MIN(dsc->p1->y, dsc->p2->y);
        a.y2 = obj->coords.y2;
        lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);

        /*Remove the masks*/
        lv_draw_mask_free_param(&line_mask_param);
        lv_draw_mask_free_param(&fade_mask_param);
        lv_draw_mask_remove_id(line_mask_id);
        lv_draw_mask_remove_id(fade_mask_id);
    }
        /*Hook the division lines too*/
    else if(dsc->part == LV_PART_MAIN) {
        if(dsc->line_dsc == NULL || dsc->p1 == NULL || dsc->p2 == NULL) return;

        /*Vertical line*/
        if(dsc->p1->x == dsc->p2->x) {
            dsc->line_dsc->color  = lv_palette_lighten(LV_PALETTE_GREY, 1);
            if(dsc->id == 3) {
                dsc->line_dsc->width  = 2;
                dsc->line_dsc->dash_gap  = 0;
                dsc->line_dsc->dash_width  = 0;
            }
            else {
                dsc->line_dsc->width = 1;
                dsc->line_dsc->dash_gap  = 6;
                dsc->line_dsc->dash_width  = 6;
            }
        }
            /*Horizontal line*/
        else {
            if(dsc->id == 2) {
                dsc->line_dsc->width  = 2;
                dsc->line_dsc->dash_gap  = 0;
                dsc->line_dsc->dash_width  = 0;
            }
            else {
                dsc->line_dsc->width = 2;
                dsc->line_dsc->dash_gap  = 6;
                dsc->line_dsc->dash_width  = 6;
            }

            if(dsc->id == 1  || dsc->id == 3) {
                dsc->line_dsc->color  = lv_palette_main(LV_PALETTE_GREEN);
            }
            else {
                dsc->line_dsc->color  = lv_palette_lighten(LV_PALETTE_GREY, 1);
            }
        }
    }
}

int fx(int x)
{
    float t = (float )x/10.0f;
    int y = (int)(80.0f*(sin((float )x/10.0f)+1));
    return y+250;
}

void test_chart(){

    size_t points = 800;
//    lv_obj_t *chart = lv_chart_create(lv_scr_act());
    lv_obj_t *chart = lv_chart_create(qview(333)->getObj());
//    lv_obj_align(chart,LV_ALIGN_DEFAULT,50,90);
    lv_obj_set_size(chart, 400, 240);
    lv_obj_set_style_line_color(chart, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);
    lv_obj_set_style_line_opa(chart, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_pad_all(chart, 0, LV_PART_MAIN);
//    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(chart, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa( chart, LV_OPA_TRANSP, LV_PART_MAIN );
    lv_obj_set_style_border_width(chart, 0, LV_PART_MAIN);
    lv_chart_set_type(chart, LV_CHART_TYPE_SCATTER);
    lv_chart_set_div_line_count(chart, 16, 12);
    lv_chart_series_t *series = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_AMBER),
                                                    LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, points);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, points);
    lv_chart_set_point_count(chart, 800);

    lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

    /*Add a faded are effect*/

    for (int i = 0; i <= points; i++) {
        int yMax = fx(i);
//            int yMax = lv_bezier3(i, 300, 1600, 120, LV_BEZIER_VAL_MAX);
        lv_chart_set_value_by_id2(chart, series, i, i, yMax);
    }

    lv_chart_refresh(chart);

}



int main(){

//    printf("main start \n");
    QuickVGL::init( 800, 480 );
//    printf("lv Init ok!\n");
    QInput::init("/dev/input/event0");
//    printf("input init ok!\n");

    ImageSet("DarkBg", "S:/Users/shezw/dev/sanboen/lvgl/QuickVGL/demo/assets/60889_flip/images/background.png");

//    testImageSlider();

//    testLabelSlider();

    testRadiusLabelSlider();
    testRadiusLabelSliderWhite();
//    testVerticalRadiusLabelSlider();

//    testRadiusLabelSlider();

//    test_demoUI();
//    testTouchSlider();
//    test_61569UI();

//    test_lvgl();

//    test_chart();


//    lv_example_style_transition();
//    lv_example_style_transitionFont();

    QuickVGL::showVersion();

    printf("start run\n");
    QUILauncher::singleton()->run( 10 );

}