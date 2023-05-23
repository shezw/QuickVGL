//
// Created by Sprite on 2023/5/12.
//

#include "qTouchSlider.hpp"

#include <utility>

static QTouchSlider * QSliders[MAX_SLIDER];

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

    container = containerView;

    std::vector<QView*> nodes = containerView->getChildren();
    size = containerView->getChildren().size();

    objs = static_cast<lv_obj_t **>(malloc(size * sizeof(lv_obj_t *)));
    size_t i = 0;

    for( const auto& view : nodes ){
        objs[i++] = view->getObj();
    }

}

QTouchSlider::~QTouchSlider() {

    if (inited) {

        lv_obj_remove_event_cb( container->getObj(), swipeStart);
        lv_obj_remove_event_cb( container->getObj(), swipeMove);
        lv_obj_remove_event_cb( container->getObj(), swipeEnd);

        if (QSliders[fd]->updateCallback)
            QSliders[fd]->updateCallback = nullptr;

        QSliders[fd] = nullptr;

        QVGLC_touchSliderDeInit(fd);
    }
    free(objs);
}

void QTouchSlider::swipeStart( lv_event_t * evt ) {
    QVGLC_touchSliderTouchStart( ((int*) evt->user_data)[0], evt );
}
void QTouchSlider::swipeMove( lv_event_t * evt ) {
    QVGLC_touchSliderTouchMove( ((int*) evt->user_data)[0], evt );
}
void QTouchSlider::swipeEnd( lv_event_t * evt ) {
    QVGLC_touchSliderTouchUp( ((int*) evt->user_data)[0], evt );
}

QTouchSlider *QTouchSlider::initSlider(  ) {
    fd = QVGLC_touchSliderInit( objs, (int)size, &config );
    inited = true;

    lv_obj_add_event_cb( container->getObj(), swipeStart, LV_EVENT_PRESSED, (void*)&fd);
    lv_obj_add_event_cb( container->getObj(), swipeMove, LV_EVENT_PRESSING, (void*)&fd);
    lv_obj_add_event_cb( container->getObj(), swipeEnd, LV_EVENT_RELEASED, (void*)&fd);

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

void QTouchSlider::elementUpdatedCall(lv_obj_t *element, int idx, int selected, int fd) {

    if( QSliders[fd] ) {
        auto *qSlider = QSliders[fd];

        qSlider->updateCallback( qSlider->container->getChild(idx), idx, selected, fd );
    }
}

QTouchSlider *QTouchSlider::onUpdate(UpdateCall call) {

    QSliders[fd] = this;
    QSliders[fd]->updateCallback = std::move(call);
    QVGLC_touchSliderRegisterElementUpdate( fd, elementUpdatedCall );
    return this;
}
