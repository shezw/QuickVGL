//
// Created by Sprite on 2023/3/8.
//

#include <thread>
#include "QUILauncher.hpp"
#include "lvgl.h"

#if OS_LINUX

#elif OS_MAC
#include <SDL2/SDL.h>
#include "sdl/sdl.h"
#endif

using namespace std;

QUILauncher::QUILauncher(){
    _exit = false;
}

void QUILauncher::handler(uint32_t ms) {
    lv_timer_handler();

#ifdef SDL_APPLE
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
#if USE_MOUSE != 0
        mouse_handler(&event);
#endif

#if USE_KEYBOARD
        keyboard_handler(&event);
#endif

#if USE_MOUSEWHEEL != 0
        mousewheel_handler(&event);
#endif
    }
#endif
}

void QUILauncher::run( uint32_t loopInMS ) const {

    while ( !_exit ) {
#if OS_LINUX
        this_thread::sleep_for(
            chrono::milliseconds( loopInMS )
        );
#elif OS_MAC
        SDL_Delay(loopInMS);
#endif
        QUILauncher::handler( loopInMS );
    }
}

void QUILauncher::kill() {
    _exit = true;
}