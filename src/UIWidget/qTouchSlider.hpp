//
// Created by Sprite on 2023/5/12.
//

#ifndef QUICKVGL_QTOUCHSLIDER_HPP
#define QUICKVGL_QTOUCHSLIDER_HPP

#include "lv_touchSlider.h"
#include "QView.hpp"

typedef std::function<void(QView * view, int index, int selected, int fd)> UpdateCall;

class QTouchSlider {

private:

    int fd;
    lv_obj_t ** objs;
    size_t size;

    QVGLC_TouchSliderConfig_t config;
    QView * container;

    bool inited = false;

    static void swipeStart( lv_event_t * evt );
    static void swipeMove( lv_event_t * evt );
    static void swipeEnd( lv_event_t * evt );
    static void elementUpdatedCall( lv_obj_t * element, int idx, int selected, int fd );

    UpdateCall updateCallback;

public:

    explicit QTouchSlider( QView * containerView );
    ~QTouchSlider();

    QTouchSlider * setDirection( QVGLC_TOUCH_SLIDER_DIRECTION direction ){ config.direction = direction; return this; }

    QTouchSlider * visibleSiblings( int maxBrother ){ config.maxBrother = maxBrother; return this; }

    QTouchSlider * dirY(){ return setDirection(TOUCH_SLIDER_DIRECTION_Y); }
    QTouchSlider * dirX(){ return setDirection(TOUCH_SLIDER_DIRECTION_X); }

    QTouchSlider * setLayout( QVGLC_TOUCH_SLIDER_LAYOUT layout ){ config.layout = layout; return this; }
    QTouchSlider * layoutLeftTop(){ return setLayout(TOUCH_SLIDER_LAYOUT_LT); }
    QTouchSlider * layoutCenter(){ return setLayout(TOUCH_SLIDER_LAYOUT_CENTER); }
    QTouchSlider * layoutRightBottom(){ return setLayout(TOUCH_SLIDER_LAYOUT_RB); }

    QTouchSlider * setCircleMode( int radius );

    QTouchSlider * loop(){ config.canLoop = true; return this; }
    QTouchSlider * notLoop(){ config.canLoop = false; return this; }

    QTouchSlider * setSize( int containerSize, int gapSize );
    QTouchSlider * setSize( int containerSize, int gapSize, int elementWidth, int elementHeight );
    QTouchSlider * setElementSize( int elementWidth, int elementHeight );

    QTouchSlider * setGapRatio( float ratio ){ config.gapRatio = ratio; return this; }
    QTouchSlider * useOpacity(){ config.useOpacity = true; return this; }
    QTouchSlider * noOpacity(){ config.useOpacity = false; return this; }

    QTouchSlider * useScale(){ config.useScale = true; return this; }
    QTouchSlider * noScale(){ config.useScale = false; return this; }

    QTouchSlider * indexTo( int idx );
    QTouchSlider * indexTo( int idx, bool useAnimation );

    QTouchSlider * initSlider();

    QTouchSlider * onUpdate( UpdateCall call );
};


#endif //QUICKVGL_QTOUCHSLIDER_HPP
