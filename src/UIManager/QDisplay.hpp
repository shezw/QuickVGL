//
// Created by Sprite on 2023/3/8.
//

#ifndef QUICKVGL_QDISPLAY_HPP
#define QUICKVGL_QDISPLAY_HPP

#include "QGraphics.hpp"
#include "QTypes.hpp"
#include "lvgl.h"
#include "lv_drivers/display/fbdev.h"

#define DEFAULT_FBDEV "/dev/fb0"

class QDisplay {

    lv_disp_t * lvDisplay = nullptr;
    lv_disp_drv_t * lvDisplayDriver;
    lv_disp_draw_buf_t * lvDispDrawBuf;

    void * lvContextBuffer;

    FrameBuffer * frameBuffer;

    static QDisplay * _defaultDisplay;

public:

    static void init();
    static void deInit();

    explicit QDisplay(lv_disp_drv_t *drv);
    QDisplay(lv_disp_drv_t *drv, uint16_t hres, uint16_t vres);

    QDisplay * Rotate();
    QDisplay * Rotate( uint16_t deg );
    static QDisplay * Default( lv_disp_drv_t *drv );

    QSize    getSize();
    uint16_t getWidth();
    uint16_t getHeight();
    static void flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);

    virtual ~QDisplay();
};


#endif //QUICKVGL_QDISPLAY_HPP
