//
// Created by sprite on 2023/3/7.
//

#ifndef QUICKVGL_QFONT_HPP
#define QUICKVGL_QFONT_HPP

#include "QInt.hpp"
#include "QString.hpp"
#include <cstdint>
#include <string>

#include "lvgl.h"

class QFont{

private:

    QInt _fontSize;
    QString fontName;
    QString _fontPath;

    lv_font_t * lvFont = nullptr;
    lv_style_t * fontStyle = nullptr;
public:

    QFont();
    explicit QFont( QString path );
    explicit QFont(const std::string& path): QFont( QString(path) ){};
    QFont( QString path, QInt fontSize );

    QFont * initFreetype();

    const lv_style_t * getStyle();

    ~QFont();
};


#endif //QUICKVGL_QFONT_HPP
