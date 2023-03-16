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

    if ( pressed > 25 )
        qview( BrightnessSettingPageID)->bg( qimage("btn-power-press") );
    else
        qview( BrightnessSettingPageID )->bg( qimage("alien") );

    qview(BrightnessLeftPanelID)->pos(left, top);
    qimage("alien")->pos(top,left);
}


void test_61569UI_addToScreen(){

    qview( BrightnessSettingPageID )->insert(
            qview(BrightnessLeftPanelID)
            ->insert( qimage("ConfirmDelColor")->pos(50,150)->show() )
            ->insert( QImage::query( IconOK_ID )->pos(20,10)->show() )
            ->bg( qimage("IconOkay") )
    );

    qview(BrightnessSettingPageID)
    ->insert(
            qimage("alien")->size(320,240)->pos(300,50)->show()
            )
    ->insert(
            qimage("btn-power-normal")->show()
            );
}

void test_61569UI(){

    printf("Hello QVGL test_61569UI\n");

    ImageSet( "ConfirmDelColor",    &testimgdst );
    ImageSet( "IconOkay",           &testimgdst2 );
    ImageSet( "alien",              "S:/home/shezw/QuickVGL/demo/assets/alien.sjpg" );
    ImageSet( "btn-power-normal",   "S:/home/shezw/QuickVGL/demo/assets/btn-power-normal.png" );
    ImageSet( "btn-power-press",    "S:/home/shezw/QuickVGL/demo/assets/btn-power-press.png" );

    QV(800,480,0,0)
    ->hash(BrightnessSettingPageID)
    ->clean()->bg(0x0C0D0EFF);

    QV(380,245,15,110)
    ->hash(BrightnessLeftPanelID)
    ->clean()->bg(0x212328FF,0xADB0BDFF)->radius(14);

    qimage("IconOkay")->copy()->hash( IconOK_ID );

    test_61569UI_addToScreen();

    /* Timer */
    auto *timer = new QTimer();                // or timer = new LvTimer();
    timer->setPeriod(15)->setCb(test_61569UI_TimerCb)->ready();

}


int main(){

    QuickVGL::init();

    test_61569UI();

    QuickVGL::showVersion();

    QUILauncher::singleton()->run( 15 );

}