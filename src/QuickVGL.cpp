//
// Created by mac on 2023/3/5.
//

#include "QuickVGL.hpp"
#include <iostream>

#if OS_LINUX
#include <pthread.h>
static pthread_t tickP;
#endif

#if USE_SDL
#include <SDL2/SDL.h>
#endif

/**
 * A task to measure the elapsed time for LittlevGL
 * @param data unused
 * @return never return
 */
static int tick_thread(void * data)
{
    (void)data;

    while(true) {
#if USE_SDL
        SDL_Delay(5);   /*Sleep for 5 millisecond*/
#else
        this_thread::sleep_for(
            chrono::milliseconds( loopInMS )
        );
#endif
        lv_tick_inc(5); /*Tell LittelvGL that 5 milliseconds were elapsed*/
    }

    return 0;
}

void QuickVGL::showVersion() {
    std::cout << "QuickVGL Version: " << QVGL_VERSION << std::endl;
}

void QuickVGL::init( const uint32_t loopInMS ) {
    lv_init();
}

void QuickVGL::init(int window_w, int window_h) {
    QuickVGL::init( QuickVGL::DefaultLoopIntervalMS );
#if USE_SDL
    QDisplay::init(window_w,window_h);
    lv_group_t * g = lv_group_create();
    lv_group_set_default(g);
    SDL_CreateThread(tick_thread, "tick", nullptr);
#else
    QDisplay::init();
    pthread_create( &tickP, tick_thread, nullptr);
#endif
}

void QuickVGL::exit() {
    QDisplay::deInit();
}

void QuickVGL::init() {
    QuickVGL::init( 0,0 );
}
