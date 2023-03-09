//
// Created by Sprite on 2023/3/8.
//

#include <thread>
#include "QUILauncher.hpp"
#include "lvgl.h"
using namespace std;

QUILauncher::QUILauncher(){
    _exit = false;
}

void QUILauncher::handler(uint32_t ms) {
    lv_tick_inc(ms);
    lv_timer_handler();
}

void QUILauncher::run( uint32_t loopInMS ) const {

    while ( !_exit ) {
        QUILauncher::handler( loopInMS );
        this_thread::sleep_for(chrono::milliseconds( loopInMS ));
    }
}

void QUILauncher::kill() {

    _exit = true;
}