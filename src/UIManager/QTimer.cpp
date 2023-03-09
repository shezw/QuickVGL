//
// Created by Sprite on 2023/3/8.
//

#include "QTimer.hpp"

QTimer::QTimer() {
    _timer = lv_timer_create_basic();
}

QTimer::~QTimer() {
}

lv_timer_t* QTimer::raw() {
    return _timer;
}
QTimer * QTimer::setCb(lv_timer_cb_t timer_cb){
    lv_timer_set_cb(_timer,timer_cb);
    return this;
}
QTimer * QTimer::del(){
    lv_timer_del(_timer);
    return this;
}
QTimer * QTimer::pause(){
    lv_timer_pause(_timer);
    return this;
}
QTimer * QTimer::resume(){
    lv_timer_resume(_timer);
    return this;
}
QTimer * QTimer::setPeriod(uint32_t period){
    lv_timer_set_period(_timer,period);
    return this;
}
QTimer * QTimer::ready(){
    lv_timer_ready(_timer);
    return this;
}
QTimer * QTimer::setRepeatCount(int32_t repeat_count){
    lv_timer_set_repeat_count(_timer,repeat_count);
    return this;
}
QTimer * QTimer::reset(){
    lv_timer_reset(_timer);
    return this;
}
lv_timer_t *QTimer::getNext() const noexcept {
    return lv_timer_get_next(_timer);

}
