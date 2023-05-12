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
    auto * qView2 = (new QView( { 250,150 }, {10,0} ))->bg({255,255,66,128});

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
//    ImageSet( "alien",              "S:/customer/res/61569_lvgl/assets/alien.sjpg" );
    ImageSet( "PowerOnIcon",        "S:/customer/res/61569_lvgl/assets/btn-power-press.png" );
    ImageSet( "PowerOffIcon",       "S:/customer/res/61569_lvgl/assets/btn-power-normal.png" );
    ImageSet( "brightness-always-on-on", "S:/customer/res/61569_lvgl/assets/brightness-always-on-on.png");
    ImageSet( "brightness-always-on-off", "S:/customer/res/61569_lvgl/assets/brightness-always-on-off.png");

    FontSet( "CNFont", "/customer/res/fonts/fzhtjw.ttf" );
    FontSet( "ENFont", "/customer/res/fonts/arial.ttf" );

//    FontSet( "CNFont", "/customer/res/fonts/fzhtjw.ttf" );
//    FontSet( "ENFont", "/customer/res/fonts/arial.ttf" );

    QV(800,1280,0,0)
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

void elementUpdatedCall( lv_obj_t * element, int idx, int selected ){

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

    auto ** elements2 = (lv_obj_t**) malloc(max_label*sizeof (lv_obj_t*));

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
            .direction      = TOUCH_SLIDER_DIRECTION_Y,
//            .direction      = TOUCH_SLIDER_DIRECTION_X,
//            .layout         = TOUCH_SLIDER_LAYOUT_LT,
            .layout         = TOUCH_SLIDER_LAYOUT_CENTER,
//            .layout         = TOUCH_SLIDER_LAYOUT_RB,
            .containerSize  = 1280,
            .objectWidth    = 120,
            .objectHeight   = 120,
            .gapSize        = 120,
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

    ImageSet("DarkBg", "S:/customer/res/60889_flip/images/background.png");

    auto * sliderContainer = QV(800,1280,0,0)->clean()->noScroll()->bg({0,0,0,0xff})->bg(qimgres("DarkBg"));

    for (int i = 0; i < 299; i++)
        sliderContainer->insert(QV( 120,120, 0,50 )->clean()->noScroll()->bg((uint32_t)0x0)->insert( QL(i)->textColor( 0xff555555 ) ));

    auto qTouchSliderController = new QTouchSlider( sliderContainer );
        qTouchSliderController->setSize( 1280, 120 )->setElementSize( 120, 120)->setCircleMode(2400)->visibleSiblings(15)->indexTo(0)->dirY()->layoutCenter()->useOpacity()->noScale()->loop()->initSlider();

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



    auto ** elements2 = (lv_obj_t**) malloc(max_label*sizeof (lv_obj_t*));

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
            .containerSize  = 800,
            .objectWidth    = 80,
            .objectHeight   = 80,
            .gapSize        = 80,
//        .canLoop        = false,
//            .useScale       = true,
            .canLoop        = true,
            .useScale       = false,
            .useOpacity     = true,
//            .gapRatio       = 0.8,
            .index          = 0,
            .inertia        = 0.8f,
            .attenuation    = 2.5f,
            .maxBrother     = 9,
            .springEdgeRatio = 0.75f,
            .radius         = 1600
    };

    int fd2 = QVGLC_touchSliderInit( elements2, max_label, &config2 );

    lv_obj_add_event_cb( container2, swipeStartContainer2, LV_EVENT_PRESSED, nullptr);
    lv_obj_add_event_cb( container2, swipeMoveContainer2, LV_EVENT_PRESSING, nullptr);
    lv_obj_add_event_cb( container2, swipeEndContainer2, LV_EVENT_RELEASED, nullptr);

    QVGLC_touchSliderRegisterElementUpdate( fd2, elementUpdatedCall );


}


int main(){

    QuickVGL::init();
    QInput::init("/dev/input/event0");


//    testImageSlider();

//    testLabelSlider();

    testRadiusLabelSlider();
//    testVerticalRadiusLabelSlider();


//    test_demoUI();
//    testTouchSlider();
//    test_61569UI();

    QuickVGL::showVersion();

    QUILauncher::singleton()->run( 10 );

}