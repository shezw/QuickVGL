//
// Created by sprite on 2023/3/7.
//
#include "QUtils.hpp"
#include "QInt.hpp"
#include "QFont.hpp"

#include <utility>

#include "lvgl.h"

#define QFONT_DEFAULT_SIZE 14


QFont::QFont() {
    _fontSize = * new QInt(QFONT_DEFAULT_SIZE);
}

QFont::QFont(QString path) {
    _fontPath = std::move(path);
    _fontSize = * new QInt(QFONT_DEFAULT_SIZE);
    initFreetype();
}

QFont::QFont(QString path, QInt fontSize) {
    _fontPath = std::move(path);
    _fontSize = * new QInt(fontSize);
    initFreetype();
}

QFont *QFont::initFreetype() {
    /*Create a font*/
    lvFont = lv_freetype_font_create( _fontPath.value().c_str(), _fontSize.value(), LV_FREETYPE_FONT_STYLE_NORMAL);

    if(!lvFont) {
        printf("freetype font create failed.\n");
        return this;
    }

    fontStyle = static_cast<lv_style_t *>(malloc(sizeof(lv_style_t)));

    /*Create style with the new font*/
    lv_style_init( fontStyle );
    lv_style_set_text_font( fontStyle, lvFont);
    lv_style_set_text_align( fontStyle, LV_TEXT_ALIGN_CENTER);

    return this;
}

QFont::~QFont() {
    if (lvFont) lv_freetype_font_del(lvFont);
    if (fontStyle) free(fontStyle);
}

const lv_style_t *QFont::getStyle() {
    return fontStyle;
}
