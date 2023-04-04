//
// Created by Sprite on 2023/4/3.
//
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lv_touchSlider.h"

#define MAX_SLIDER 8

typedef struct TOUCH_SLIDER_STATE {

    /* 方向 X,Y */
    TOUCH_SLIDER_DIRECTION direction;

    /* 元素间隔 */
    int gapSize;

    /* 是否支持循环 */
    bool canLoop;

    /* 默认选中项 */
    unsigned char index;

    /* 滚动惯性 0-1 惯性越大 滚动距离越远 */
    float inertia;

    /* 衰减 用于控制滑动时长的衰减，即随着速度增长时长增长减缓 建议: 2 - 3 */
    float attenuation;

    lv_obj_t ** objs;
    int total;


    /* 记忆down事件位置 */
    int startPosition;

    /* 当前事件的位置 */
    int currentPosition;

} TouchSliderState ;

static TouchSliderState * fds[ MAX_SLIDER ];

int findSliderFd(){
    for (int i = 0; i < MAX_SLIDER; i++) {
        if ( fds[i] == NULL ){
            return i;
        }
    }
    return -1;
}

void updateSingleElementPos( lv_obj_t * obj, TouchSliderState * state, int idx ){

    int moving   = 0;
    int elOffset = idx - state->index;

    if (state->direction == TOUCH_SLIDER_DIRECTION_X){
        lv_obj_set_x( obj, elOffset * state->gapSize);
    }else{
        lv_obj_set_y( obj, elOffset * state->gapSize);
    }
}

void updateElementsPosition( int fd, lv_event_t * evt ){

    TouchSliderState * state = fds[fd];
    lv_obj_t ** objs = state->objs;

    if ( evt != NULL ){
        lv_point_t point_p;
        lv_indev_get_point(lv_indev_get_act(), &point_p);
        state->currentPosition = state->direction==TOUCH_SLIDER_DIRECTION_X ? point_p.x : point_p.y;
    }

    for (int i = 0; i < state->total; i++) {

        updateSingleElementPos( objs[i], state, i );
    }
}


int  touchSliderInit( lv_obj_t ** objs, int size, TouchSliderConfig_t * config ) {

    int fd = findSliderFd();
    assert( fd>-1 );

    TouchSliderState * state = (TouchSliderState *) malloc( sizeof( TouchSliderState ) );
    memset( state, 0, sizeof( TouchSliderState ) );

    state->direction = config->direction;
    state->gapSize   = config->gapSize;
    state->total     = size;
    state->objs      = objs;

    state->canLoop   = config->canLoop;
    state->attenuation = config->attenuation;
    state->inertia   = config->inertia;

    state->startPosition = 0;

    fds[fd] = state;

    updateElementsPosition(fd,NULL);

    return fd;
}
