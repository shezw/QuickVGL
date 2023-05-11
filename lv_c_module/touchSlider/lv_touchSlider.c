//
// Created by Sprite on 2023/4/3.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include "math.h"
#include "lv_touchSlider.h"

/**
 * @brief	qvglc_get_systime_ms(void)
 * 处理器开机经过的时间，毫秒级
 * @return	返回时间值
 */
QVGLC_timestamp qvglc_get_systime_ms(void) {

    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return milliseconds;
}

/**
 * @brief	qvglc_timer_tickcount_get(void)
 * 处理器开机经过的tick数
 * @return	返回的tick数
 */
QVGLC_timestamp qvglc_timer_tickcount_get(void) {

    struct timespec ts;
    unsigned int theTick;
    clock_gettime( CLOCK_REALTIME, &ts );
    theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;

    return theTick;
}

typedef struct QVGLC_TOUCH_SLIDER_STATE {

    int fd;

    /* 效果类型 */
    QVGLC_TOUCH_SLIDER_FX_MODE fx;

    /* 方向 X,Y */
    QVGLC_TOUCH_SLIDER_DIRECTION direction;

    /* 是否与触控反向 */
    bool touchReverse;

    /* 对齐 LT,Center,RB */
    QVGLC_TOUCH_SLIDER_LAYOUT layout;

    /* 容器大小 */
    int containerSize;

    /* 对象大小 */
    int objectWidth;
    int objectHeight;

    /* 元素间隔 */
    int gapSize;

    /* 用于控制元素间的间隙变化  与高亮元素距离有关 */
    float gapRatio;

    /* 是否支持循环 */
    bool canLoop;

    /* 是否使用缩放 */
    bool useScale;

    /* 是否支持透明度 */
    bool useOpacity;

    /* 默认选中项 */
    int index;

    /* 边缘弹性系数 */
    float springEdgeRatio;

    /* 速度系数 用于控制对于滑动速度的倍率   比值越大，对于快滑的效果成倍加快 默认2 */
    float velocityRatio;

    /* 滚动惯性 0-1 惯性越大 滚动距离越远 */
    float inertia;

    /* 衰减 用于控制滑动时长的衰减，即随着速度增长时长增长减缓 建议: 2 - 3 */
    float attenuation;

    lv_obj_t ** objs;
    int total;

    int maxBrother;

    /* 用于弧面效果的圆弧半径 */
    int radius;
    float angle;

    /* 用于动画计算的基准延时 MS (200ms) */
    QVGLC_timestamp baseDelay;

    /* 记忆down事件位置 */
    int startPosition;

    /* 当前事件的位置 */
    int currentPosition;

    QVGLC_TouchSliderUpdated updatedCall;
    QVGLC_TouchSliderElementUpdated elementUpdatedCall;
    QVGLC_TouchSliderSelectionUpdated selectionUpdatedCall;

    bool isMoving;

    /* 实际用于 */
    int offset;

    /* 实时位移 */
    int moving;

    /* 实时元素位移 */
    int movingOffset;

    /* 实时目标位移 */
    int movingTarget;

    int * objectOffsets;


    /**
     * 用于动画和速度处理
     */
    QVGLC_timestamp start_t;
    QVGLC_timestamp last_t;
    QVGLC_timestamp now_t;
    QVGLC_timestamp duration;
    QVGLC_timestamp duration_total;
    float velocity;
    int movingDirection;

} QVGLC_TouchSliderState ;


struct animationOption {
    int fd;
    QVGLC_TouchSliderState * state;
    QVGLC_timestamp duration;
    int fps;
    int * xs;
    int * ys;

    int end_x;
    int end_y;
};

static pthread_t animation_threads[ MAX_SLIDER ];
static bool  run_animation_threads[ MAX_SLIDER ] = {0};


static QVGLC_TouchSliderState * fds[ MAX_SLIDER ];

int findSliderFd(){
    for (int i = 0; i < MAX_SLIDER; i++) {
        if ( fds[i] == NULL ){
            return i;
        }
    }
    return -1;
}

static void animationPosXCall(void * var, int32_t v)
{
    lv_obj_set_x(var, (short)v);
}

static void animationPosYCall(void * var, int32_t v)
{
    lv_obj_set_y(var, (short)v);
}

void animationToXY( lv_obj_t * obj, int x, int y, QVGLC_timestamp duration ) {

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, lv_obj_get_x(obj), x);
    lv_anim_set_time(&a, duration);
    lv_anim_set_exec_cb(&a, animationPosXCall );
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);

    lv_anim_set_exec_cb(&a, animationPosYCall );
    lv_anim_set_values(&a, lv_obj_get_y(obj), y);
    lv_anim_start(&a);

}

void animationToPos( lv_obj_t * obj, int pos, QVGLC_TOUCH_SLIDER_DIRECTION direction, QVGLC_timestamp duration ){
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, (direction==TOUCH_SLIDER_DIRECTION_X?lv_obj_get_x(obj):lv_obj_get_y(obj)), pos);
    lv_anim_set_time(&a, duration);
    lv_anim_set_exec_cb(&a, (direction==TOUCH_SLIDER_DIRECTION_X?animationPosXCall:animationPosYCall) );
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);
}

void animationToX( lv_obj_t * obj, int x, QVGLC_timestamp duration ){

    animationToPos( obj, x, TOUCH_SLIDER_DIRECTION_X, duration );
}

void animationToY( lv_obj_t * obj, int y, QVGLC_timestamp duration ){

    animationToPos( obj, y, TOUCH_SLIDER_DIRECTION_Y, duration );
}

void updateSingleElementPos( lv_obj_t * obj, QVGLC_TouchSliderState * state, int idx, bool useAnimation ){

    int elOffset = state->objectOffsets[idx];
    int layoutOffset = (int)state->layout * state->containerSize/2;
    if (state->layout==TOUCH_SLIDER_LAYOUT_CENTER)
        layoutOffset -= state->gapSize/2;
    if (state->layout==TOUCH_SLIDER_LAYOUT_RB)
        layoutOffset -= state->gapSize;

    QVGLC_timestamp duration = state->baseDelay;
    if ( state->velocity < 1 ){
        duration = 100;
    }

    //    printf("ISLOOP %d, index %d, idx %d, elOffset %d movingOffset %d\n",state->canLoop,state->index,idx,elOffset,movingOffset);

    if (state->fx==TOUCH_SLIDER_FX_MODE_CIRCLE){ // 弧形特效

        int px, py, rx, ry, movingOffsetX, movingOffsetY = 0; // 计算圆心位置

        px = state->direction==TOUCH_SLIDER_DIRECTION_X ? layoutOffset  : state->radius;
        py = state->direction==TOUCH_SLIDER_DIRECTION_X ? state->radius : layoutOffset;

        rx = (int) (floorf( cosf(((float) elOffset + (float)state->moving/(float)(state->gapSize) ) * state->angle ) * (float)state->radius ) * (state->direction==TOUCH_SLIDER_DIRECTION_X ? powf( state->gapRatio, abs(elOffset) ) : 1));
        ry = (int) (floorf( sinf(((float) elOffset + (float)state->moving/(float)(state->gapSize) ) * state->angle ) * (float)state->radius ) * (state->direction==TOUCH_SLIDER_DIRECTION_Y ? powf( state->gapRatio, abs(elOffset) ) : 1));

        movingOffsetX = px + (state->direction==TOUCH_SLIDER_DIRECTION_X?rx:-rx);
        movingOffsetY = py + (state->direction==TOUCH_SLIDER_DIRECTION_Y?ry:-ry);

        if (useAnimation) {
            animationToXY(obj,movingOffsetX,movingOffsetY, duration);
        }else{
            lv_obj_set_x(obj, (short)movingOffsetX);
            lv_obj_set_y(obj, (short)movingOffsetY);
        }

    }else
    {
        int movingOffset = (int)((float)elOffset * ((float)state->gapSize* powf( state->gapRatio, (float)abs(elOffset) ))) + state->moving + layoutOffset;

        if (state->direction == TOUCH_SLIDER_DIRECTION_X) {
            if (useAnimation) {
                animationToX(obj, (short) movingOffset, duration);
            }else
                lv_obj_set_x(obj, (short)movingOffset);
        } else {
            if (useAnimation)
                animationToY(obj, (short)movingOffset, duration);
            else
                lv_obj_set_y(obj, (short)movingOffset);
        }
    }


    if (state->useScale) {

        lv_obj_t * img = lv_obj_get_child( obj, 0 );
        lv_img_set_zoom(img,256-16* abs(elOffset));

//        printf( "to W %d, H %d\n", state->objectWidth - (abs(elOffset) * 50), state->objectHeight - (abs(elOffset) * 52) );
//        lv_obj_set_width(obj, state->objectWidth - (abs(elOffset) * 30));
//        lv_obj_set_height(obj, state->objectHeight - (abs(elOffset) * 32));
    }

    lv_obj_move_to_index( obj, state->total - ( elOffset<0 ? elOffset*-2 : elOffset ) );


    if (abs(elOffset) <= state->maxBrother )
        lv_obj_clear_flag( obj, LV_OBJ_FLAG_HIDDEN );
    else
        lv_obj_add_flag( obj, LV_OBJ_FLAG_HIDDEN );

    if (state->elementUpdatedCall)
        state->elementUpdatedCall( obj, idx, state->index );
//    printf("updateSingleElementPos %d %d  index_offset %d, should hide %d  \n", idx, movingOffset, state->index_offset,abs(elOffset) <= state->maxBrother );
}

void updateSingleElementZoom( lv_obj_t * obj, QVGLC_TouchSliderState * state, int idx ){

    unsigned int zoom = 256;

    if (state->canLoop) {
        zoom = (idx - state->total / 2 - state->index) * 16 ;
    }else {
        zoom -= (idx - state->index) * 16;
    }

    lv_img_set_zoom( obj, zoom);

}

// 计算循环位移
void calculateElementsLoop( QVGLC_TouchSliderState * state ) {

    if( state->index < 0 )
        state->index = state->canLoop ? state->total-1 : 0;

    if( state->index > state->total-1 )
        state->index = state->canLoop ? 0 : state->total-1;
}

// 计算元素偏移
void calculateElementsOffset( QVGLC_TouchSliderState * state ) {

    int centerDistance = 0 ,endDistance = 0, rate = 0;

    for (int i = 0; i < state->total; i++) {

        centerDistance = i - state->index;
        endDistance    = centerDistance - state->total;

        rate = centerDistance;

        if (state->canLoop) {

            if( abs( rate ) > state->maxBrother ){

                rate = centerDistance>0 ? state->maxBrother+1 : -state->maxBrother-1;
            }

            if( abs( centerDistance ) > (state->total/2) ){

                rate =  state->total%2 == 1
                        ? (centerDistance - ( centerDistance>0 ? state->total : -state->total ))
                        : (i > (state->total/2) ? endDistance : state->total+centerDistance) ;
            }
        }

        state->objectOffsets[i] = (state->touchReverse ? -1 : 1) * rate;
    }
}

void refreshView( QVGLC_TouchSliderState * state, bool useAnimation ){

//    calculateSelection(state);

    for (int i = 0; i < state->total; i++) {

        updateSingleElementPos(state->objs[i], state, i, useAnimation);
    }

//    printf("%d,%d   Active: %d\n",point_p.x,point_p.y, state->index);

    if (state->updatedCall)
        state->updatedCall( state->index );
}


void swipeToTarget( QVGLC_TouchSliderState * state, bool useAnimation ){

    state->index  = state->movingTarget;

//    checkSpeed(state);
    calculateElementsLoop(state);

    calculateElementsOffset( state );
    refreshView(state, useAnimation);

    /**

        let now = new Date().getTime();
        this.duration = this.timeStamp>0 ? now - this.timeStamp : this.animationDuration;
        this.timeStamp = now;

        if( this.delay > 0 && this.delayStamp>0 && (now-this.delayStamp) < this.delay ) {
            return;
        }else{
            this.duration = now - this.delayStamp;
            this.delayStamp = now;
        }

        if( this.transitionLock && this.duration < this.speed*this.animationDuration ){
            this.transitionLock = false;
            this.indexTo(targetIndex);
            return;
        }

        this.targetDirection = targetIndex - this.current;

        this.current = targetIndex;

        this._checkSpeed();               //  检测事件间隔 低于阈值取消动画
        this._calculateElementsLoop();    //  调整循环游标
        if( !this.strictMode && this.virtualElement) {
            this._prepareVirtualElements();   //  准备虚拟元素 初始位置
        }
        this._calculateElementsOffset();  //  计算位移
        this._refreshView();              //  刷新视图

     */

    if ( state->selectionUpdatedCall )
        state->selectionUpdatedCall( state->index );
}


void movingAdjustment( QVGLC_TouchSliderState * state ){

    refreshView( state, false );
}


void calculateMovingDelta( QVGLC_TouchSliderState * state ){

    if ( !state->isMoving ) return;
//    printf("calculateMovingDelta [ %d - %d ]  %d \n", state->currentPosition , state->startPosition, state->moving );

    state->moving       = (state->touchReverse ? -1 : 1) * (state->currentPosition - state->startPosition );
    state->movingOffset = (int) roundf( (float)state->moving / (float)state->gapSize ) * -1;
    state->movingTarget = state->movingOffset + state->index;

    if (!state->canLoop){

        if ((state->index==0 && state->moving>1) || (state->movingTarget>(state->total-1) && state->moving<-1)){

            state->movingOffset = 0;
            state->movingTarget = state->index;

            state->moving = (state->moving>0 ? 1 : -1) * ( (int)pow(abs(state->moving), state->springEdgeRatio ) );
        }
    }

    // 判断是否需要进行焦点切换
    if ( state->movingOffset != 0 ){
        state->startPosition = state->currentPosition+state->moving+(state->moving>0?-5:5);
        state->moving = -state->moving;

        swipeToTarget( state, false );

    }else{

        if ( abs(state->moving) < 3 ) return;

        movingAdjustment( state );
    }

    /**
    this._touchPointUpdated( moving )
    */

}

void QVGLC_touchSliderToIndex(int sliderFD, int targetIndex, bool useAnimation){

    QVGLC_TouchSliderState * state = fds[sliderFD];

    state->movingTarget = targetIndex;

    swipeToTarget( state, useAnimation );

}


/**
 * 切换到对应索引的元素
 * @param sliderFD
 * @param targetIndex
 */
void QVGLC_touchSliderSwipeTo( int sliderFD, int targetIndex ){

    QVGLC_touchSliderToIndex( sliderFD, targetIndex, true );

}

/**
 * 切换到上一个
 * @param sliderFD
 */
void QVGLC_touchSliderToPrev( int sliderFD, bool useAnimation ) {

    QVGLC_TouchSliderState * state = fds[sliderFD];

    if (state->canLoop){
        state->movingTarget = (state->index+state->total-1) % state->total;
    }else{
        if (state->index<=0) state->movingTarget = 0;
    }

    state->velocity = 1;
    swipeToTarget( state, useAnimation );

}

/**
 * 切换到下一个
 * @param sliderFD
 */
void QVGLC_touchSliderToNext( int sliderFD, bool useAnimation ) {

    QVGLC_TouchSliderState * state = fds[sliderFD];

    if (state->canLoop){
        state->movingTarget = state->index+1 % state->total;
    }else{
        if (state->index>=state->total-1) state->movingTarget = state->total-1;
    }

    state->velocity = 1;
    swipeToTarget( state, useAnimation );

}

void QVGLC_touchSliderSwipePrev( int sliderFD ) {

    QVGLC_touchSliderToPrev( sliderFD, true );
}

void QVGLC_touchSliderSwitchPrev( int sliderFD ) {

    QVGLC_touchSliderToPrev( sliderFD, false );
}

void QVGLC_touchSliderSwipeNext( int sliderFD ) {

    QVGLC_touchSliderToNext( sliderFD, true );
}

void QVGLC_touchSliderSwitchNext( int sliderFD ) {

    QVGLC_touchSliderToNext( sliderFD, false );
}


bool QVGLC_touchSliderDeInit( int sliderFD ){

    free(fds[sliderFD]->objectOffsets);
    free(fds[sliderFD]);
    fds[sliderFD] = NULL;
    return true;
}

int QVGLC_touchSliderInit( lv_obj_t ** objs, int size, QVGLC_TouchSliderConfig_t * config ) {

    int fd = findSliderFd();
    assert( fd>-1 );

    QVGLC_TouchSliderState * state = (QVGLC_TouchSliderState *) malloc( sizeof( QVGLC_TouchSliderState ) );
    memset( state, 0, sizeof( QVGLC_TouchSliderState ) );

    state->fd               = fd;

    state->startPosition    = 0;
    state->total            = size;
    state->objs             = objs;

    state->fx               = config->fx;
    state->direction        = config->direction;
    state->index            = config->index;
    state->layout           = config->layout;
    state->gapSize          = config->gapSize;

    state->gapRatio         = config->gapRatio;
    if (state->gapRatio==0.0)
        state->gapRatio = 1;

    state->velocityRatio    = config->velocityRatio;
    if (state->velocityRatio==0.0)
        state->velocityRatio = 2;

    state->springEdgeRatio  = config->springEdgeRatio;
    if (state->springEdgeRatio==0)
        state->springEdgeRatio = 0.75f;

    state->containerSize    = config->containerSize;
    state->objectWidth      = config->objectWidth;
    state->objectHeight     = config->objectHeight;
    state->maxBrother       = config->maxBrother;
    state->canLoop          = config->canLoop;
    state->attenuation      = config->attenuation;
    state->inertia          = config->inertia;
    state->useScale         = config->useScale;
    state->useOpacity       = config->useOpacity;
    state->radius           = config->radius;

    state->baseDelay        = config->baseDelay > 0 ? config->baseDelay : 200;

    if (state->fx==TOUCH_SLIDER_FX_MODE_CIRCLE && state->radius)
    state->angle            = (float) state->gapSize / (float) state->radius ;
    else
    assert(state->fx==TOUCH_SLIDER_FX_MODE_NORMAL);

    state->updatedCall        = NULL;
    state->elementUpdatedCall = NULL;

    state->isMoving         = false;

    state->objectOffsets    = malloc(sizeof(int)*state->total);

    fds[fd] = state;

    QVGLC_touchSliderSwipeTo( fd, state->index );

//    refreshView(state);
//    updateElementsPosition(fd,NULL);

    return fd;
}

void QVGLC_touchSliderTouchStart( int sliderFD, lv_event_t * evt ) {

    QVGLC_TouchSliderState * state = fds[sliderFD];

    lv_point_t point_p;
    lv_indev_get_point(lv_indev_get_act(), &point_p);

    if ( evt != NULL ) {

        if( evt->code == LV_EVENT_PRESSED ) {
            state->isMoving = true;
            state->start_t  = qvglc_get_systime_ms();
            state->currentPosition = state->startPosition = ((state->direction == TOUCH_SLIDER_DIRECTION_X )? point_p.x : point_p.y);
            printf("Down Event %d,%d  %d Active: %d\n",point_p.x,point_p.y, state->startPosition, state->index);
        }
    }

}

void QVGLC_touchSliderTouchMove( int sliderFD, lv_event_t * evt ) {

    QVGLC_TouchSliderState * state = fds[sliderFD];

    if (!state->isMoving) return;

    lv_point_t point_p;
    lv_indev_get_point(lv_indev_get_act(), &point_p);

    if ( evt != NULL ) {

        if( evt->code == LV_EVENT_PRESSING ) {

            state->last_t               = state->now_t;
            state->now_t                = qvglc_get_systime_ms();
            state->currentPosition      = state->direction == TOUCH_SLIDER_DIRECTION_X ? point_p.x : point_p.y;
        }else
            return;
    }

    calculateMovingDelta( state );

}

bool isAnimationRunning( int fd );

void exitAnimation( int fd );

void * runningAnimationThread( void * arg );

void startAnimationThread( QVGLC_TouchSliderState * state );

void QVGLC_touchSliderTouchUp( int sliderFD, lv_event_t * evt ) {

    QVGLC_TouchSliderState * state = fds[sliderFD];

    if (!state->isMoving) return;

    lv_point_t point_p;
    lv_indev_get_point(lv_indev_get_act(), &point_p);

    if ( evt != NULL ) {

        if( evt->code == LV_EVENT_RELEASED ) {
            state->now_t         = qvglc_get_systime_ms();
            state->duration      = state->now_t-state->last_t;
            state->duration_total= state->now_t-state->start_t;
            state->movingDirection = state->currentPosition > state->startPosition ? 1 : -1;
            state->velocity      = (float)((unsigned int)abs(state->currentPosition-state->startPosition) * 2) / (float)state->duration_total;
            state->velocity      = state->velocity<1 ? state->velocity : powf(state->velocity, state->velocityRatio);

            printf("Up Event %d,%d   Active: %d duration(%u) duration_total(%u) ( %d, %d, %d ) velocity(%f) \n",
                   point_p.x,point_p.y, state->index, state->duration,
                   state->duration_total,
                   state->currentPosition, state->startPosition, (state->currentPosition-state->startPosition),
                   state->velocity
            );

            state->startPosition = state->currentPosition = 0;
            state->now_t = state->last_t = state->start_t = 0;
            state->isMoving = false;
        }
    }

    state->moving = 0;

    if ( state->velocity <= 1 )
        refreshView(state,true);
    else
        startAnimationThread(state);
}

void QVGLC_touchSliderRegisterUpdate( int sliderFD, QVGLC_TouchSliderUpdated updatedCall ) {

    if (fds[sliderFD]) {
        fds[sliderFD]->updatedCall = updatedCall;
        refreshView(fds[sliderFD],false);
    }
}

void QVGLC_touchSliderRegisterElementUpdate( int sliderFD, QVGLC_TouchSliderElementUpdated elementUpdatedCall ) {

    if (fds[sliderFD]) {
        fds[sliderFD]->elementUpdatedCall = elementUpdatedCall;
        refreshView(fds[sliderFD],false);
    }
}

bool isAnimationRunning( int fd ){
    return run_animation_threads[fd];
}

void exitAnimation( int fd ){
    run_animation_threads[fd] = false;
}

void * runningAnimationThread( void * arg ){

    struct animationOption * cmd = (struct animationOption *) arg;

    QVGLC_timestamp totalTime = 0;
    QVGLC_timestamp interval  = 1000/cmd->fps;
    QVGLC_TouchSliderState * state = cmd->state;

    while ( animation_threads[cmd->fd] && totalTime<cmd->duration){ // 动画未执行完

        printf("Try running animation\n");

        if ( state->movingDirection>0 )
            QVGLC_touchSliderSwitchNext( state->fd );
        else
            QVGLC_touchSliderSwitchPrev( state->fd );

        totalTime += interval;
        usleep( interval * 1000 );
    }

    animation_threads[cmd->fd] = false;
    printf("Stop running animation\n");

    (void) arg;

    return 0;
}

void startAnimationThread( QVGLC_TouchSliderState * state ) {

    int sliderFD = state->fd;

    if (state->velocity<=1.0f)
        return;

    if (isAnimationRunning(sliderFD))
        exitAnimation(sliderFD);

    struct animationOption * cmd = malloc( sizeof (struct animationOption) );
    memset( cmd, 0, sizeof(struct animationOption));

    cmd->fd = sliderFD;
    cmd->state = state;
    cmd->fps   = 1000/LV_DISP_DEF_REFR_PERIOD;

    cmd->duration = (QVGLC_timestamp)floorf(powf(state->velocity,state->inertia)) * state->baseDelay;

    int ret = pthread_create( &animation_threads[sliderFD] , NULL, runningAnimationThread, (void *)cmd);
//    pthread_join( animation_threads[cmd->fd], NULL );

}
