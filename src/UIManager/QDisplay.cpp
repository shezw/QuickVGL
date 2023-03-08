//
// Created by Sprite on 2023/3/8.
//

#include "QDisplay.hpp"
#include <memory>

#include "lv_drivers/display/fbdev.h"

char * bufferAddr;

void testFlush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p)
{
    static unsigned char color = 255;
    printf("Draw buffer\n");
    memset( bufferAddr, color++, 800 * 480 * 4 );

    // TODO
    lv_disp_flush_ready(drv);
}

QDisplay::QDisplay(lv_disp_drv_t *drv) : QDisplay(drv, 800, 480) {
}

QDisplay::QDisplay(lv_disp_drv_t *drv, uint16_t hres, uint16_t vres) {

    frameBuffer = new FrameBuffer("/dev/fb0",0);

    bufferAddr = frameBuffer->screen_buffer;

    lvDisplayDriver = (lv_disp_drv_t *) malloc( sizeof(lv_disp_drv_t));
    bzero(lvDisplayDriver,sizeof(lv_disp_drv_t));

    lv_disp_drv_init( lvDisplayDriver );

    lvDispDrawBuf = (lv_disp_draw_buf_t *) malloc( sizeof(lv_disp_draw_buf_t));
    bzero(lvDispDrawBuf,sizeof(lv_disp_draw_buf_t));

    lvDisplayDriver->flush_cb = testFlush;

    lv_disp_draw_buf_init( lvDispDrawBuf, frameBuffer->fb_draw_buffer, nullptr, frameBuffer->screen_size );

    lvDisplay = (_lv_disp_t *) malloc( sizeof(_lv_disp_t));
    bzero(lvDisplay,sizeof(_lv_disp_t));


//    static lv_color_t buf1_1[MONITOR_HOR_RES * 100];
//	static lv_color_t buf1_2[MONITOR_HOR_RES * 100];
//	lv_disp_draw_buf_init(disp_buf1.get(), buf1_1, buf1_2, MONITOR_HOR_RES * 100);
//
//        /* Initialize with basic configuration*/
        if (!drv) {
            /*Create a display*/
            lvDisplayDriver->draw_buf = lvDispDrawBuf;
            lvDisplayDriver->antialiasing = 1;
            lvDisplayDriver->sw_rotate = 1;
            lvDisplayDriver->hor_res = getWidth();
            lvDisplayDriver->ver_res = getHeight();
            lvDisplayDriver->rotated = LV_DISP_ROT_NONE;
        } else
            memcpy(lvDisplayDriver, drv, sizeof(lv_disp_drv_t));
//        disp.reset(lv_disp_drv_register(disp_drv.get()));
//        void(0);
    lvDisplay = lv_disp_drv_register( lvDisplayDriver );

}


QDisplay::~QDisplay() {
//        ~*frameBuffer;
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
    // TODO
    lv_disp_flush_ready(drv);
}

uint16_t QDisplay::getWidth() {
    return getSize().w;
}

uint16_t QDisplay::getHeight() {
    return getSize().h;
}

QSize QDisplay::getSize() {
    return frameBuffer->getScreenSize();
}
