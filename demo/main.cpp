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
QLabel * qLabel;
QView  * qView1;

static void QUart_init(void)
{
    char header = 0xff;
    hv_uart_config_t config = {
        .uartPath = (char*)"/dev/ttyS0",
        .baudRate = 9600,
        .nBits = 8,
        .nEvent = 'N',
        .nStop = 1,
        
        .headerLen = 1, 
        .header = &header,
        .datalenIndex = 4,
        .lengthCompensate = 6,//13-datalen(7)
        .footerLen = 0,
        .footer = NULL,
    };
    hv_uart_fd fd3 = hv_uart_init(&config);
    // hv_uart_reg_verify(fd3,frameVerifyFunc);
    // hv_uart_reg_recviver(fd3,frameUartRecviver);
    char sendbuf[2] = {0x0f,0x0a};
    hv_uart_send(fd3,sendbuf,sizeof(sendbuf));
}

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


int main(){

    printf("Hello QVGL\n");

    QuickVGL::init();
    QUart_init();


    auto display = new QDisplay( nullptr );

    auto * body = new QView( { 450, 300 }, {150,50} );

    qView1 = new QView( { 150,150 }, {0,0} );
    auto * qView2 = new QView( { 250,150 }, {10,0} );

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


//    printf("%p\n",UIManager::singleton()->getByID(344));

//    QLabel label1 = new QLabel( "sprite" );
//    label1.text("hello").append("world").show();
//    label1.text("").hide();

    QuickVGL::showVersion();

    QUILauncher::singleton()->run( 15 );

}