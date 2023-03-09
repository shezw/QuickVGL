//
// Created by mac on 2023/3/5.
//

#include "QuickVGL.hpp"

#include "cstdio"
#include "main.hpp"

typedef enum {

    HomePageID,
    CurrentTempID

} AppID ;

QString * labelText;

/* Callback for Timer pressed */
static void TimerCb(lv_timer_t *timer) {
    static char pressed = 0;
    pressed++;

    *labelText = std::to_string( pressed );
}


int main(){

    printf("Hello QVGL\n");

    QuickVGL::init();

    auto display = new QDisplay( nullptr );

    auto * qView = new QView( { 450, 300 }, {150,50} );

    auto * qView1 = new QView( { 150,150 }, {0,0} );
    auto * qView2 = new QView( { 250,150 }, {10,0} );

    qView->insert( qView1 );
    qView->insert( qView2 );

    qView2->pos({ 120, 150 });

    labelText = new QString("Hello QLabel");
    auto * qLabel = new QLabel( labelText );

    qView2->insert( qLabel );

    *labelText = " Test Qstring binding ";

//    initSecondPage();

//    QPage::getByID( HomePageID )->hide();

//    qqLabel.append("b").append("c");

//    QString::getByID( CurrentTempID )->set( "27.6" );

//    QInt::getByID( CurrentTempID );

    printf("%s\n", QString::getByID( CurrentTempID )->set( "27.6" )->value().c_str() );


    /* Timer */
    auto *timer = new QTimer();                // or timer = new LvTimer();
    timer->setPeriod(1000)->
            setCb(TimerCb)->
            ready();


//    printf("%p\n",UIManager::singleton()->getByID(344));

//    QLabel label1 = new QLabel( "sprite" );
//    label1.text("hello").append("world").show();
//    label1.text("").hide();

    QuickVGL::showVersion();

    QUILauncher::singleton()->run( 15 );

}