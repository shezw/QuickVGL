//
// Created by sprite on 2023/3/6.
//
#include "lvgl.h"
#include "QLabel.hpp"

QLabel::QLabel(QString *t) {
    lvObj = lv_label_create( lv_scr_act() );
    text( _text = t );
    bind(t);
}

QLabel * QLabel::bind( QString * t ){
    t->setterCall = [this]( QString * txt ){
        this->text( txt );
    };
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
