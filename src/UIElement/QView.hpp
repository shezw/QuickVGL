//
// Created by mac on 2023/3/5.
//

#ifndef QUICKVGL_QVIEW_HPP
#define QUICKVGL_QVIEW_HPP

#include <vector>
#include "QTypes.hpp"

#include "lvgl.h"

enum class QViewState{
    DEFAULT     =  0x0000,
    CHECKED     =  0x0001,
    FOCUSED     =  0x0002,
    FOCUS_KEY   =  0x0004,
    EDITED      =  0x0008,
    HOVERED     =  0x0010,
    PRESSED     =  0x0020,
    SCROLLED    =  0x0040,
    DISABLED    =  0x0080,

    USER_1      =  0x1000,
    USER_2      =  0x2000,
    USER_3      =  0x4000,
    USER_4      =  0x8000,

    any = 0xFFFF,    /**< Special value can be used in some functions to target all states*/
} ;

class QView {

    QColor _bgColor{};
    QColor _tintColor{};
    QSize  _size{};
    QPosition _position{};

    // child elements
    std::vector<QView*> nodes;

protected:
    lv_obj_t * lvObj;
public:

    QView();

    ~QView();

    QView( QSize size, QPosition pos );

    static QView * getByID( QAppIDType id );

    /**
        Styles
     */

    QPosition pos();
    QSize     size();
    QColor    bg( QViewState forState );
    QColor    bg();
    QColor    tintColor();

    QView * size( QSize size );
    QView * pos( QPosition pos );
    QView * bg( QColor bgColor, QViewState forState );
    QView * bg( QColor bgColor );
    QView * tintColor( QColor color );

    /**
            Layouts
     */

    QView * insert( QView * view );
    QView * batchInsert( QView **view );

    QView * hide();
    QView * show();

};



#endif //QUICKVGL_QVIEW_HPP
