//
// Created by sprite on 2023/3/6.
//
#include "lvgl.h"
#include "QLabel.hpp"
#include "QUIManager.hpp"

QLabel::QLabel(QString *t) {
    lvObj = lv_label_create( lv_scr_act() );
    text( _text = t );
    bind(t);
}

QLabel * QLabel::bind( QString * t ){
    t->combineLambda( [this]( QString * txt ){
        this->text( txt );
    }, this->id );
    return this;
}

QLabel * QLabel::text(QString * t) {
    _text = t;
    lv_label_set_text( lvObj, t->value().c_str() );
    return this;
}

QLabel * QLabel::append(QString * t) {
    return this;
}

QLabel *QLabel::font(QFont *ft) {

//    /*Create a label with the new style*/
//    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_obj_add_style( lvObj , ft->getStyle(), 0);
//    lv_label_set_text(label, "Hello world\nI'm a font created with FreeType");
//    lv_obj_center(label);
//
    return this;
}

QLabel *QLabel::query(QHashID id) {
    return static_cast<QLabel *>(QUIManager::singleton()->query(id));
}
