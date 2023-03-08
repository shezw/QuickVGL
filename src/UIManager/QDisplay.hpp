//
// Created by Sprite on 2023/3/8.
//

#ifndef QUICKVGL_QDISPLAY_HPP
#define QUICKVGL_QDISPLAY_HPP

#include "QGraphics.hpp"
#include "QTypes.hpp"
#include "lvgl.h"
#include "lv_drivers/display/fbdev.h"

class QDisplay {

    lv_disp_t * lvDisplay;
    lv_disp_drv_t * lvDisplayDriver;
    lv_disp_draw_buf_t * lvDispDrawBuf;

    FrameBuffer * frameBuffer;

public:

    explicit QDisplay(lv_disp_drv_t *drv);
    QDisplay(lv_disp_drv_t *drv, uint16_t hres, uint16_t vres);

    QDisplay * Rotate();
    QDisplay * Rotate( uint16_t deg);

    QSize    getSize();
    uint16_t getWidth();
    uint16_t getHeight();
    void     flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);

    virtual ~QDisplay();
};


#endif //QUICKVGL_QDISPLAY_HPP
