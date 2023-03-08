//
// Created by mac on 2023/3/5.
//

#include "qvgl.hpp"

#include "cstdio"
#include "main.hpp"

typedef enum {

    HomePageID,
    CurrentTempID

} AppID ;


void initHomePage(){

    QView * qView = new QView();

//    qView->insert(reinterpret_cast<struct QView &>(qView));
//
//    auto * HomePage = new QPage();

//    HomePage.inset( QLabel("25.6c") );

//    HomePage->insert(reinterpret_cast<QView &>(qView));
}




int main(){

    printf("Hello QVGL\n");

//    QUIManager::init();

//    initHomePage();

//    initSecondPage();

//    QPage::getByID( HomePageID )->hide();

//    qqLabel.append("b").append("c");

//    QString::getByID( CurrentTempID )->set( "27.6" );

//    QInt::getByID( CurrentTempID );

    printf("%s\n", QString::getByID( CurrentTempID )->set( "27.6" )->value().c_str() );


//    printf("%p\n",UIManager::singleton()->getByID(344));



//    QLabel label1 = new QLabel( "sprite" );
//    label1.text("hello").append("world").show();
//    label1.text("").hide();

    showQVersion();
}