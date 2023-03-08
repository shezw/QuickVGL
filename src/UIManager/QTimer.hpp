//
// Created by Sprite on 2023/3/8.
//

#ifndef QUICKVGL_QTIMER_HPP
#define QUICKVGL_QTIMER_HPP

#include "lvgl.h"

class QTimer {
private:

public:
    QTimer ();
    virtual ~QTimer ();
    lv_timer_t* raw();
    QTimer & setCb(lv_timer_cb_t timer_cb);
    QTimer & del();
    QTimer & pause();
    QTimer & resume();
    QTimer & setPeriod(uint32_t period);
    QTimer & ready();
    QTimer & setRepeatCount(int32_t repeat_count);
    QTimer & reset();
    lv_timer_t *getNext() const noexcept ;
};


#endif //QUICKVGL_QTIMER_HPP
