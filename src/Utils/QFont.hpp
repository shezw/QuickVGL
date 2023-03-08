//
// Created by sprite on 2023/3/7.
//

#ifndef QUICKVGL_QFONT_HPP
#define QUICKVGL_QFONT_HPP

#include "QValue.hpp"
#include <cstdint>
#include <string>

class QFont{

private:

    QInt fontSize;
    QString fontName;
    QString path;

public:

    QFont();
    explicit QFont( QString path );
    QFont( QString path, QInt fontSize );


};


#endif //QUICKVGL_QFONT_HPP
