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

QLabel::QLabel( int i) {
    lvObj = lv_label_create( lv_scr_act() );
    _text = nullptr;
    lv_label_set_text_fmt(lvObj, "%d", i);
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

    lv_obj_add_style( lvObj , ft->getStyle(), 0);
    return this;
}

QLabel *QLabel::query(QHashID id) {
    return static_cast<QLabel *>(QUIManager::singleton()->query(id));
}

QLabel *QLabel::text(QInt *t) {
    lv_label_set_text_fmt(lvObj, "%d", t->value());
    return this;
}

QLabel *QLabel::textColor( QColor color) {
    lv_obj_set_style_text_color( lvObj, {color.b,color.g,color.r}, 0 );
    return this;
}

QLabel *QLabel::textColor(uint32_t hexColor) {
    lv_obj_set_style_text_color( lvObj, lv_color_hex(hexColor), 0 );
    return this;
}
