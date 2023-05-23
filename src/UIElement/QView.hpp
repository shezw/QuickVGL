//
// Created by mac on 2023/3/5.
//

#ifndef QUICKVGL_QVIEW_HPP
#define QUICKVGL_QVIEW_HPP

#include <vector>
#include "QTypes.hpp"
#include "QImageResource.hpp"

#include "lvgl.h"

enum class QViewState{
    DEFAULT     =  LV_PART_MAIN | LV_STATE_DEFAULT,
    CHECKED     =  0x000001,
    FOCUSED     =  0x000002,
    FOCUS_KEY   =  0x000004,
    EDITED      =  0x000008,
    HOVERED     =  0x000010,
    PRESSED     =  0x000020,
    SCROLLED    =  0x000040,
    DISABLED    =  0x000080,

    USER_1      =  0x1000,
    USER_2      =  0x2000,
    USER_3      =  0x4000,
    USER_4      =  0x8000,

    any = 0xFFFFFF,    /**< Special value can be used in some functions to target all states*/
} ;

typedef enum class NoneType{
    QViewNoneView,
    QViewNoneImage,
    QViewNoneLabel
} QViewNone;

class QImage;
class QImageResource;

class QView {

    QColor _bgColor{};
    QColor _tintColor{};
    QSize  _size{};
    QPosition _position{};
    QImageResource * _bgImage{};
    QImageResource * _checkBgImage{};

    // child elements
    std::vector<QView*> nodes;

protected:
    lv_obj_t * lvObj{};
    bool _none = false;
public:
    static QView *none();
    bool isNone() const { return _none; }

    QHashID id{};

    QView();
    explicit QView(QViewNone);

    ~QView();

    lv_obj_t * getObj(){ return lvObj; };

    QView( QSize size, QPosition pos );

    static QView * query( QHashID id );
    static QView * create( uint16_t w, uint16_t h, int16_t x, int16_t y );
    static QView * create( uint16_t w, uint16_t h );

    std::vector<QView*> getChildren() const;
    QView * getChild( int index ) const;

    /**
        Styles
     */

    QPosition pos();
    QSize     size();
    QColor    bg( QViewState forState );
    QColor    bg();
    QColor    tintColor();

    QView * size( QSize size );
    QView * size( uint16_t w, uint16_t h) { return size({w,h}); }
    QView * pos( QPosition pos );
    QView * pos( int16_t x, int16_t y) { return pos({x,y}); }
    QView * bg( QColor bgColor, QViewState forState );
    QView * bg( QColor bgColor, QColor checkColor );
    QView * bg( uint32_t hexRGBA );
    QView * bg( uint32_t hexRGBA, QViewState forState);
    QView * bg( uint32_t bgColor, uint32_t checkColor );
    QView * bg( QColor bgColor );
    QView * bg( QImageResource * img );
    QView * bg( QImageResource * img, QViewState forState);
    QView * bg(  QImageResource * img, QImageResource * checkImg );
    QView * checkBg(uint32_t hexColor);
    QView * checkBg(QColor bgColor);
    QView * checkBg( QImageResource * img );
    QImageResource *checkBgImg();

    QView * tintColor( QColor color );
    QImageResource * bgImg();

    /**
            Layouts
     */

    QView * insert( QView * view );
    QView * batchInsert( QView **view );

    QView * hide();
    QView * show();

    QView * noScroll();

    QView * radius(uint16_t r);

    QView * checkable();
    QView * unCheckable();

    QView * noBorder();
    QView * border( uint16_t weight );

    QView * padding( uint16_t top, uint16_t right, uint16_t bottom, uint16_t left );
    QView * paddingTop( uint16_t v );
    QView * paddingRight( uint16_t v );
    QView * paddingBottom( uint16_t v );
    QView * paddingLeft( uint16_t v );
    QView * noPadding();


    /**
     * UI Management
     */
    QView * hash( QHashID vid );

    QView *clean();
};



#endif //QUICKVGL_QVIEW_HPP
