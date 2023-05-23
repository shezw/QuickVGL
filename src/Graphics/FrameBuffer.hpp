//
// Created by Sprite on 2023/3/8.
//

#ifndef QUICKVGL_FRAMEBUFFER_HPP
#define QUICKVGL_FRAMEBUFFER_HPP

#if OS_LINUX

#include "configs/config.h"
#include "QTypes.hpp"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <setjmp.h>
#include <stdexcept>
#include <unistd.h>
#include <cstring>
#if OS(LINUX)
#include <linux/fb.h>
#endif
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <cstring>
#include <cstdio>

#if CHIP(SSD202)

#include "mstarFb.h"
#include "mi_sys.h"

#endif

class FrameBuffer {
#if OS(LINUX)
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
#endif
    int fbfd{};

    uint8_t buffer_index = 0;
    bool enableDoubleBuf = false;
    uint8_t currentScreen = 0;

    void draw_line_low(QRect l, QColor color) const;
    void draw_line_high(QRect l, QColor color) const;
public:

    char * fb_draw_buffer{};
    char * screen_buffer{};
    long int screen_size{};

    explicit FrameBuffer(const char *device);
    FrameBuffer(const char *device, bool useDB );
    ~FrameBuffer();

    void draw_pixel(QPosition point, QColor color) const;
    void draw_rectangle(QRect rect, QColor color) const;
//    void draw_image(QPosition point, const Image & image);

    void draw_line(QRect l, QColor color);
    void draw_framed_rect(QRect l, QColor color);
//    void draw_image_rect(QRect wnd, QRect pos, const Image & image);
    void updateAlpha( unsigned char alpha );

    void clear_screen(QColor color);

    QSize getScreenSize();

#if !USE_MEMCPY
    void dmaCopyScreen();
#endif // !USE_MEMCPY

    void swap_buffer();
};

#endif
#endif //QUICKVGL_FRAMEBUFFER_HPP
