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
#if (LVGL_VERSION_MAJOR > 8)
    lvFont = lv_freetype_font_create( _fontPath.value().c_str(), _fontSize.value(), LV_FREETYPE_FONT_STYLE_NORMAL);
#else
    // FIXME
    info = static_cast<lv_ft_info_t *>(malloc(sizeof(lv_ft_info_t)));
    memset(info,0,sizeof(lv_ft_info_t));
    /*Create a font*/
    /*FreeType uses C standard file system, so no driver letter is required.*/

    info->name   = _fontPath.value().c_str();
    info->weight = _fontSize.value();
    info->style  = FT_FONT_STYLE_NORMAL;
    info->mem = nullptr;
    if(!lv_ft_font_init(info)) {
        printf("lv_ft_font_init failed (%s)\n",info->name );
        return this;
        LV_LOG_ERROR("create failed.");
    }
    lvFont = info->font;
#endif

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
#if (LVGL_VERSION_MAJOR>8)
    if (lvFont) lv_freetype_font_del(lvFont);
#else
    if (info) free(info);
#endif
    if (fontStyle) free(fontStyle);
}

#if (LVGL_VERSION_MAJOR==9)
const
#endif
lv_style_t *QFont::getStyle() {
    return fontStyle;
}
