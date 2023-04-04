//
// Created by Sprite on 2023/4/3.
//

#ifndef QUICKVGL_LV_TOUCHSLIDER_H
#define QUICKVGL_LV_TOUCHSLIDER_H

#include "lvgl.h"

typedef enum {
    TOUCH_SLIDER_DIRECTION_X,
    TOUCH_SLIDER_DIRECTION_Y
} TOUCH_SLIDER_DIRECTION;


typedef struct TOUCH_SLIDER_CONFIG {

    /* 方向 X,Y */
    TOUCH_SLIDER_DIRECTION direction;

    /* 元素间隔  */
    int gapSize;

    /* 是否支持循环 true/false */
    bool canLoop;

    /* 默认选中项 0 */
    unsigned char index;

    /* 滚动惯性 0-1 惯性越大 滚动距离越远 (0.8) */
    float inertia;

    /* 衰减 用于控制滑动时长的衰减，即随着速度增长时长增长减缓 建议: 2 - 3 */
    float attenuation;

    /* 显示(相邻)元素数量 >0 */
    int maxBrother;

    /* 弹性边缘  在触及边缘的时候，越拉阻尼越大  true/false */
    bool springEdge;



} TouchSliderConfig_t;


/**
 *
 * @param objs      所有元素的指针
 * @param size      元素数量
 * @param config    配置
 * @return sliderFD
 */
int  touchSliderInit( lv_obj_t ** objs, int size, TouchSliderConfig_t * config );

bool touchSliderDeInit( int sliderFD );

void touchSliderTouchStart( lv_event_t * evt );
void touchSliderTouchMove( lv_event_t * evt );
void touchSliderTouchUp( lv_event_t * evt );

#endif //QUICKVGL_LV_TOUCHSLIDER_H
