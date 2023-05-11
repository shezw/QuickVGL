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
#if (LVGL_VERSION_MAJOR<9)
    lv_ft_info_t * info = nullptr;
#endif
public:

    QFont();
    explicit QFont( QString path );
    explicit QFont(const std::string& path): QFont( QString(path) ){};
    QFont( QString path, QInt fontSize );

    QFont * initFreetype();

#if (LVGL_VERSION_MAJOR==9)
    const
#endif
    lv_style_t * getStyle();
    ~QFont();
};


#endif //QUICKVGL_QFONT_HPP
