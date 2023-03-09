//
// Created by Sprite on 2023/3/8.
//

#include "FrameBuffer.hpp"
#include <cstdio>

#if CHIP(SSD202)

#include "mstarFb.h"
#include "mi_sys.h"

#endif

FrameBuffer::FrameBuffer(const char *device ) {

    FrameBuffer( device, false );
}

QSize FrameBuffer::getScreenSize() {
    return {
            (uint16_t)vinfo.xres,
            (uint16_t)vinfo.yres
    };
}

FrameBuffer::FrameBuffer(const char *device, bool useDB) {

    enableDoubleBuf = useDB;
    currentScreen = 0;

    fbfd = open(device, O_RDWR);
    if (fbfd == -1) {
        throw std::runtime_error("cannot open framebuffer device");
    }
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        throw std::runtime_error("error reading fixed information");
    }
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        throw std::runtime_error("error reading variable information");
    }

    printf("FrameBuffer Info (%d,%d, %d) PHYAddr:(%lx)\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, finfo.smem_start );

    screen_size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // init screen buffer virtual address
    screen_buffer = (char *)mmap(nullptr, screen_size * (enableDoubleBuf?2:1), PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

    if (*(int *)screen_buffer == -1) {
        throw std::runtime_error("failed to map framebuffer device to memory");
    }

    fb_draw_buffer = enableDoubleBuf ? &screen_buffer[ screen_size * ( (currentScreen+1) % 2 ) ] : new char [screen_size] ;

    printf("FrameBuffer Done! \n" );

}

FrameBuffer::~FrameBuffer() {

    if( !enableDoubleBuf ) delete [] fb_draw_buffer;
    munmap(screen_buffer, screen_size * (enableDoubleBuf ? 2 : 1) );

    close(fbfd);
}

/*
 * @dep
 */
void FrameBuffer::updateAlpha( unsigned char alpha ){

    for (uint32_t y = 0; y < vinfo.yres; y++) {
        for (uint32_t x = 0; x < vinfo.xres; x++) {

            *(screen_buffer + (y*finfo.line_length + x*4) + 3) = alpha;

        }
    }
}

void FrameBuffer::draw_pixel(QPosition point, QColor color) {

    uint32_t location = point.x * (vinfo.bits_per_pixel / 8) + point.y  * finfo.line_length;

    *(fb_draw_buffer + location + 0) = color.b;
    *(fb_draw_buffer + location + 1) = color.g;
    *(fb_draw_buffer + location + 2) = color.r;
    *(fb_draw_buffer + location + 3) = color.a;

}

void FrameBuffer::draw_rectangle(QRect rect, QColor color) {

    for (int16_t y = rect.p1.y; y < rect.p2.y; ++y) {
        for (int16_t x = rect.p1.x; x < rect.p2.x; ++x) {
            draw_pixel({x, y}, color);
        }
    }
}

//void FrameBuffer::draw_image(Point point, const Image &image) {
//    for (uint32_t y = 0; y < image.height; ++y) {
//        for (uint32_t x = 0; x < image.width; ++x) {
//            draw_pixel({point.x + x, point.y + y}, image.get_pixel(x, y));
//        }
//    }
//}

void FrameBuffer::draw_line_low(QRect l, QColor color) {
    auto dx = (int)l.p2.x - (int)l.p1.x;
    auto dy = (int)l.p2.y - (int)l.p1.y;
    auto yi = 0;
    if (dy < 0) {
        dy *= -1;
        yi = -1;
    } else {
        yi = 1;
    }
    auto d = (dy << 1) - dx;
    auto y = l.p1.y;
    for (auto x = l.p1.x; x < l.p2.x; x++) {
        draw_pixel({x, y}, color);
        if (d > 0) {
            y += yi;
            d -= dx << 1;
        }
        d += dy << 1;
    }
}

void FrameBuffer::draw_line_high(QRect l, QColor color) {
    auto dx = (int)l.p2.x - (int)l.p1.x;
    auto dy = (int)l.p2.y - (int)l.p1.y;
    auto xi = 0;
    if (dx < 0) {
        dx *= -1;
        xi = -1;
    } else {
        xi = 1;
    }
    auto d = (dx << 1) - dy;
    auto x = l.p1.x;
    for (auto y = l.p1.y; y < l.p2.y; y++) {
        draw_pixel({x, y}, color);
        if (d > 0) {
            x += xi;
            d -= dy << 1;
        }
        d += dx << 1;
    }
}

void FrameBuffer::draw_line(QRect l, QColor color) {
    if (std::abs((int)l.p2.y - (int)l.p1.y) < std::abs((int)l.p2.x - (int)l.p1.x)) {
        if (l.p1.x > l.p2.x) {
            draw_line_low({l.p2.x, l.p2.y, l.p1.x, l.p1.y}, color);
        } else {
            draw_line_low({l.p1.x, l.p1.y, l.p2.x, l.p2.y}, color);
        }
    } else {
        if (l.p1.y > l.p2.y) {
            draw_line_high({l.p2.x, l.p2.y, l.p1.x, l.p1.y}, color);
        } else {
            draw_line_high({l.p1.x, l.p1.y, l.p2.x, l.p2.y}, color);
        }
    }
}

void FrameBuffer::draw_framed_rect(QRect l, QColor color) {
    QPosition p1 = {l.p1.x, l.p1.y};
    QPosition p2 = {static_cast<int16_t>(p1.x + l.p2.x), p1.y};
    QPosition p3 = {p2.x, static_cast<int16_t>(p1.y + l.p2.y)};
    QPosition p4 = {p1.x, p3.y};

    draw_line({p1, p2}, color);
    draw_line({p2, p3}, color);
    draw_line({p3, p4}, color);
    draw_line({p4, p1}, color);
}

//void FrameBuffer::draw_image_rect(QRect wnd, QRect pos, const Image & image) {
//    for (uint32_t y = 0; y < wnd.p2.y; ++y) {
//        for (uint32_t x = 0; x < wnd.p2.x; ++x) {
//            draw_pixel({pos.p1.x + x, pos.p1.y + y}, image.get_pixel(wnd.p1.x + x, wnd.p1.y + y));
//        }
//    }
//}

void FrameBuffer::clear_screen(QColor color) {

//        memset( fb_draw_buffer, (uint32_t), screen_size/4 );
//        memcpy(screen_buffer, fb_draw_buffer, screen_size);
//        memset(fb_draw_buffer,color.red,screen_size);

#if USE_DMA

    #if CHIP(SSD202)
    MI_SYS_MemsetPa(
        !currentScreen ? finfo.smem_start + screen_size : finfo.smem_start,
        ((color.b<<24)+(color.g<<16)+(color.r<<8)+(color.a)),
        screen_size
    );
#endif

#else

    draw_rectangle({0, 0, (int16_t)vinfo.xres, (int16_t)vinfo.yres}, color);

#endif

}

#if !USE_MEMCPY

#if CHIP(SSD202)

void FrameBuffer::dmaCopyScreen(){

    uint32_t from   = finfo.smem_start + (currentScreen ? screen_size : 0);
    uint32_t to     = finfo.smem_start + (currentScreen ? 0 : screen_size);

    MI_SYS_MemcpyPa( from, to , screen_size );

}

#endif

#endif // !USE_MEMCPY


void FrameBuffer::swap_buffer() {

    if ( enableDoubleBuf ) {

        vinfo.yoffset = currentScreen ? 0 : vinfo.yres; // 0的时候显示1屏

        if (ioctl(fbfd, FBIOPAN_DISPLAY, &vinfo) < 0) {
            throw std::runtime_error("error reading variable information");
        }

#if USE_MEMCPY
        memcpy( fb_draw_buffer, screen_buffer, screen_size );
#else
        dmaCopyScreen();
#endif
        fb_draw_buffer =  &screen_buffer[ screen_size * currentScreen ];
        currentScreen = currentScreen ? 0 : 1;
    }else{
        memcpy(screen_buffer, fb_draw_buffer, screen_size);
    }
}