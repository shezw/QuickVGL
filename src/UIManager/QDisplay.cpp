//
// Created by Sprite on 2023/3/8.
//

#include "QDisplay.hpp"
#include <memory>

#include "lv_drivers/display/fbdev.h"

QDisplay * QDisplay::_defaultDisplay = nullptr;

QDisplay::QDisplay(lv_disp_drv_t *drv) : QDisplay(drv, 800, 480) {
}

QDisplay::QDisplay(lv_disp_drv_t *drv, uint16_t hres, uint16_t vres) {

#if OS_LINUX

    frameBuffer = new FrameBuffer(DEFAULT_FBDEV, false );
    lvContextBuffer = malloc( frameBuffer->screen_size );

#elif OS_MAC
    sdl_init( hres, vres );
    lvContextBuffer = malloc( SDL_HOR_RES * SDL_VER_RES * 4 );
#endif


    lvDisplayDriver = (lv_disp_drv_t *) malloc( sizeof(lv_disp_drv_t));
    bzero(lvDisplayDriver,sizeof(lv_disp_drv_t));

    lv_disp_drv_init( lvDisplayDriver );

    lvDispDrawBuf = (lv_disp_draw_buf_t *) malloc( sizeof(lv_disp_draw_buf_t));
    bzero(lvDispDrawBuf,sizeof(lv_disp_draw_buf_t));

#if OS_LINUX
    lvDisplayDriver->flush_cb = QDisplay::flush;

    lv_disp_draw_buf_init( lvDispDrawBuf, lvContextBuffer, nullptr, frameBuffer->screen_size );
#elif OS_MAC

    lvDisplayDriver->flush_cb = sdl_display_flush;
    lv_disp_draw_buf_init( lvDispDrawBuf, lvContextBuffer, nullptr, SDL_HOR_RES * SDL_VER_RES);

#endif

    if (!lvDisplay) lvDisplay = (_lv_disp_t *) malloc( sizeof(_lv_disp_t));
    bzero(lvDisplay,sizeof(_lv_disp_t));

    /* Initialize with basic configuration*/
    if (!drv) {

        /*Create a display*/
        lvDisplayDriver->draw_buf = lvDispDrawBuf;
        lvDisplayDriver->antialiasing = 1;
        lvDisplayDriver->sw_rotate = 1;
        lvDisplayDriver->hor_res = (int16_t) getWidth();
        lvDisplayDriver->ver_res = (int16_t) getHeight();
//        lvDisplayDriver->full_refresh = 1;
        lvDisplayDriver->rotated = LV_DISP_ROT_NONE;
//        lvDisplayDriver->rotated = LV_DISP_ROT_90;
        lvDisplayDriver->user_data = this;
        lvDisplayDriver->draw_ctx_size = 1024*1024*4;
    } else
        memcpy(lvDisplayDriver, drv, sizeof(lv_disp_drv_t));

    lvDisplay = lv_disp_drv_register( lvDisplayDriver );

    lv_theme_t * th = lv_theme_default_init(lvDisplay, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
    lv_disp_set_theme(lvDisplay, th);

    lv_group_t * g = lv_group_create();
    lv_group_set_default(g);

    if ( !_defaultDisplay ) _defaultDisplay = this;

}


QDisplay::~QDisplay() {
    free(lvDisplay);
#if OS_LINUX
    delete frameBuffer;
#endif
}

QDisplay * QDisplay::Rotate() {

    lv_disp_rot_t rot = LV_DISP_ROT_NONE;
    lv_disp_rot_t future_rot = LV_DISP_ROT_NONE;
    rot = lv_disp_get_rotation(lvDisplay);

    switch (rot) {
        case LV_DISP_ROT_NONE:
            future_rot = LV_DISP_ROT_90;
            break;

        case LV_DISP_ROT_90:
            future_rot = LV_DISP_ROT_180;
            break;

        case LV_DISP_ROT_180:
            future_rot = LV_DISP_ROT_270;
            break;

        case LV_DISP_ROT_270:
            future_rot = LV_DISP_ROT_NONE;
            break;
    }

    lv_disp_set_rotation(lvDisplay, future_rot);

    return this;

}

QDisplay * QDisplay::Rotate(uint16_t deg) {

    uint32_t rotstep = 1;

    if (deg % 90 == 0) {
        rotstep = deg / 90;
        for (int i = 0; i < rotstep; i++)
            Rotate();
    }
    return this;
}

void QDisplay::flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p)
{
    auto * display = static_cast<QDisplay *>(drv->user_data);

#if OS_LINUX
    memcpy( display->frameBuffer->fb_draw_buffer, color_p, (area->x2-area->x1+1)*(area->y2-area->y1)*4 );
    display->frameBuffer->swap_buffer();
#endif

    // TODO Now only support FullRefresh
    lv_disp_flush_ready(drv);

}

uint16_t QDisplay::getWidth() {
    return getSize().w;
}

uint16_t QDisplay::getHeight() {
    return getSize().h;
}

QSize QDisplay::getSize() {
#if OS_LINUX
    return frameBuffer->getScreenSize();
#elif USE_SDL || USE_SDL_GPU
    return {SDL_HOR_RES,SDL_VER_RES};
#else
    return {800,480};
#endif
}

QDisplay *QDisplay::Default( lv_disp_drv_t *drv ) {
    if ( !QDisplay::_defaultDisplay ){
        QDisplay::_defaultDisplay = new QDisplay( drv );
    }
    return QDisplay::_defaultDisplay;
}

QDisplay *QDisplay::Default( lv_disp_drv_t *drv, int w, int h ) {
    if ( !QDisplay::_defaultDisplay ){
        QDisplay::_defaultDisplay = new QDisplay( drv, w, h );
    }
    return QDisplay::_defaultDisplay;
}

void QDisplay::init() {
    QDisplay::Default(nullptr);
}

void QDisplay::deInit() {
    delete _defaultDisplay;
}

void QDisplay::init(int w, int h) {
    QDisplay::Default(nullptr, w, h);
}
