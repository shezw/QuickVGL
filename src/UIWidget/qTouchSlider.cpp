//
// Created by Sprite on 2023/5/12.
//

#include "qTouchSlider.hpp"

QTouchSlider::QTouchSlider( QView * containerView ) {

    config = {
        .fx             = TOUCH_SLIDER_FX_MODE_NORMAL,
        .direction      = TOUCH_SLIDER_DIRECTION_X,
        .layout         = TOUCH_SLIDER_LAYOUT_CENTER,

        .containerSize  = 800,
        .objectWidth    = 200,
        .objectHeight   = 200,

        .gapSize        = 200,

        .canLoop        = true,
        .useScale       = false,
        .useOpacity     = true,

        .index          = 0,

        .maxBrother     = 2,
    };

    std::vector<QView*> nodes = containerView->getChildren();
    size = containerView->getChildren().size();

    objs = static_cast<lv_obj_t **>(malloc(size * sizeof(lv_obj_t *)));
    size_t i = 0;

    for( const auto& view : nodes ){
        objs[i++] = view->getObj();
    }

}

QTouchSlider::~QTouchSlider() {

    QVGLC_touchSliderDeInit(fd);
    free(objs);
}

QTouchSlider *QTouchSlider::initSlider() {
    fd = QVGLC_touchSliderInit( objs, (int)size, &config );
    inited = true;
    return this;
}

QTouchSlider *QTouchSlider::setCircleMode(int radius) {
    config.fx     = TOUCH_SLIDER_FX_MODE_CIRCLE;
    config.radius = radius;
    return this;
}

QTouchSlider *QTouchSlider::setSize(int containerSize, int gapSize) {
    config.containerSize = containerSize;
    config.gapSize = gapSize;
    return this;
}

QTouchSlider *QTouchSlider::setSize(int containerSize, int gapSize, int elementWidth, int elementHeight) {
    config.containerSize    = containerSize;
    config.gapSize          = gapSize;
    config.objectWidth      = elementWidth;
    config.objectHeight     = elementHeight;
    return this;
}

QTouchSlider *QTouchSlider::setElementSize(int elementWidth, int elementHeight) {
    config.objectWidth      = elementWidth;
    config.objectHeight     = elementHeight;
    return this;
}

QTouchSlider *QTouchSlider::indexTo(int idx){

    return indexTo(idx, false);
}

QTouchSlider *QTouchSlider::indexTo(int idx, bool useAnimation) {
    if (!inited)
        config.index = idx;
    else{
        QVGLC_touchSliderToIndex( fd, idx, useAnimation );
    }
    return this;
}
