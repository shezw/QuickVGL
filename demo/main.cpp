//
// Created by mac on 2023/3/5.
//

#include "QuickVGL.hpp"

#include "cstdio"
#include "testimg.h"
#include "signal.h"
#include "main.hpp"

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
        qview(BrightnessSettingPageID)->bg(qimgres("alien"));
        qlabel(LightOnTitleText)->text( new QString("同时点亮 | 同时熄灭"));
    }

    qview(BrightnessLeftPanelID)->pos(left, top);
    qimage("alien")->pos(top,left);
    // FIXME
    // 通过qimage获取到的image在特定情况下会出现无法拿到data的问题。暂不清楚原因
}


void test_61569UI_addToScreen(){

    qview( BrightnessSettingPageID )
        ->insert(
            qview(BrightnessLeftPanelID)
            ->insert( qimage("ConfirmDelColor")->pos(50,150)->show() )
            ->bg(qimgres("IconOkay") )
    );

    qview(BrightnessSettingPageID)
        ->insert(
            qimage("alien")->size({320, 240})->pos(300,50)->show())
        ->insert(
            qimage("PowerOffIcon")->show());
}

void test_61569UI(){

    printf("Hello QVGL test_61569UI\n");

    ImageSet( "ConfirmDelColor",    &testimgdst );
    ImageSet( "IconOkay",           &testimgdst2 );
    ImageSet( "alien",              "S:/home/shezw/QuickVGL/demo/assets/61569_lvgl/assets/alien.sjpg" );
    ImageSet( "PowerOnIcon",        "S:/home/shezw/QuickVGL/demo/assets/61569_lvgl/assets/btn-power-press.png" );
    ImageSet( "PowerOffIcon",       "S:/home/shezw/QuickVGL/demo/assets/61569_lvgl/assets/btn-power-normal.png" );
    ImageSet( "brightness-always-on-on", "S:/home/shezw/QuickVGL/demo/assets/61569_lvgl/assets/brightness-always-on-on.png");
    ImageSet( "brightness-always-on-off", "S:/home/shezw/QuickVGL/demo/assets/61569_lvgl/assets/brightness-always-on-off.png");

    FontSet( "CNFont", "/home/shezw/QuickVGL/demo/assets/fonts/fzhtjw.ttf" );
    FontSet( "ENFont", "/home/shezw/QuickVGL/demo/assets/fonts/arial.ttf" );

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


int main(){

    QuickVGL::init();

    test_61569UI();

    QuickVGL::showVersion();

    QUILauncher::singleton()->run( 15 );

}