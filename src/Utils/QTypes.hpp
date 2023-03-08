//
// Created by sprite on 2023/3/6.
//

#ifndef QUICKVGL_QTYPES_HPP
#define QUICKVGL_QTYPES_HPP

#include <cstdint>

typedef uint32_t QAppIDType;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} QColor;

typedef struct {
    uint16_t w;
    uint16_t h;
} QSize;

typedef struct {
    int16_t x;
    int16_t y;
} QPosition;

typedef struct {
    QPosition point;
    QSize size;
} QRect;

#endif //QUICKVGL_QTYPES_HPP
