//
// Created by Sprite on 2023/4/3.
//

#ifndef QUICKVGL_LV_TOUCHSLIDER_H
#define QUICKVGL_LV_TOUCHSLIDER_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SLIDER 8
#define DEFAULT_DELAY 200 // MS

typedef unsigned int QVGLC_timestamp;

typedef enum {
    TOUCH_SLIDER_DIRECTION_X,
    TOUCH_SLIDER_DIRECTION_Y
} QVGLC_TOUCH_SLIDER_DIRECTION;

typedef enum {
    TOUCH_SLIDER_LAYOUT_LT = 0, // 左/上对齐
    TOUCH_SLIDER_LAYOUT_CENTER = 1, // 居中
    TOUCH_SLIDER_LAYOUT_RB = 2// 右/下对齐
} QVGLC_TOUCH_SLIDER_LAYOUT;

typedef enum {
    TOUCH_SLIDER_FX_MODE_NORMAL = 0, // 普通列表
    TOUCH_SLIDER_FX_MODE_CIRCLE = 1  // 弧面列表
} QVGLC_TOUCH_SLIDER_FX_MODE;

typedef void (* QVGLC_TouchSliderElementUpdated )( lv_obj_t * element, int idx, int selected ) ;
typedef void (* QVGLC_TouchSliderUpdated )( int selected ) ;
typedef void (* QVGLC_TouchSliderSelectionUpdated)( int selected );

typedef struct QVGL_TOUCH_SLIDER_CONFIG {

    /* 效果类型 */
    QVGLC_TOUCH_SLIDER_FX_MODE fx;

    /* 方向 X,Y */
    QVGLC_TOUCH_SLIDER_DIRECTION direction;

    /* 对齐 LT,Center,RB */
    QVGLC_TOUCH_SLIDER_LAYOUT layout;

    /* 容器大小 */
    int containerSize;

    /* 对象大小 */
    int objectWidth;
    int objectHeight;

    /* 元素间隔  */
    int gapSize;

    /* 用于控制元素间的间隙变化  与高亮元素距离有关 */
    float gapRatio;

    /* 是否支持循环 true/false */
    bool canLoop;

    /* 是否支持缩放 */
    bool useScale;

    /* 是否支持透明度 */
    bool useOpacity;

    /* 默认选中项 0 */
    unsigned char index;

    /* 速度系数 用于控制对于滑动速度的倍率   比值越大，对于快滑的效果成倍加快 默认2 */
    float velocityRatio;

    /* 滚动惯性 0-1 惯性越大 滚动距离越远 (0.8) */
    float inertia;

    /* 衰减 用于控制滑动时长的衰减，即随着速度增长时长增长减缓 建议: 2 - 3 */
    float attenuation;

    /* 显示(相邻)元素数量 >0 */
    int maxBrother;

    /* 弹性边缘  在触及边缘的时候，越拉阻尼越大  默认0.75 1时无阻尼 （大于1时阻尼反效果） */
    float springEdgeRatio;

    /* 用于弧面效果的圆弧半径 */
    int radius;

    /* 用于动画计算的基准延时 MS (200ms) */
    QVGLC_timestamp baseDelay;

} QVGLC_TouchSliderConfig_t;

/**
 *
 * @param objs      所有元素的指针
 * @param size      元素数量
 * @param config    配置
 * @return sliderFD
 */
int  QVGLC_touchSliderInit( lv_obj_t ** objs, int size, QVGLC_TouchSliderConfig_t * config );

bool QVGLC_touchSliderDeInit( int sliderFD );

void QVGLC_touchSliderTouchStart( int sliderFD, lv_event_t * evt );
void QVGLC_touchSliderTouchMove( int sliderFD, lv_event_t * evt );
void QVGLC_touchSliderTouchUp( int sliderFD, lv_event_t * evt );

void QVGLC_touchSliderRegisterUpdate( int sliderFD, QVGLC_TouchSliderUpdated updatedCall );
void QVGLC_touchSliderRegisterElementUpdate( int sliderFD, QVGLC_TouchSliderElementUpdated elementUpdatedCall );


/**
 * 切换到对应索引的元素
 * @param sliderFD
 * @param targetIndex
 */
void QVGLC_touchSliderSwipeTo( int sliderFD, int targetIndex );

/**
 * 切换到上一个
 * @param sliderFD
 */
void QVGLC_touchSliderToPrev( int sliderFD, bool useAnimation );

/**
 * 切换到下一个
 * @param sliderFD
 */
void QVGLC_touchSliderToNext( int sliderFD, bool useAnimation );

// 滑动切换(动画)
void QVGLC_touchSliderSwipePrev( int sliderFD );
void QVGLC_touchSliderSwipeNext( int sliderFD );

// 直接切换(无动画)
void QVGLC_touchSliderSwitchPrev( int sliderFD );
void QVGLC_touchSliderSwitchNext( int sliderFD );

#endif //QUICKVGL_LV_TOUCHSLIDER_H

#ifdef __cplusplus
} /*extern "C"*/
#endif
