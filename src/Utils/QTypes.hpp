//
// Created by sprite on 2023/3/6.
//

#ifndef QUICKVGL_QTYPES_HPP
#define QUICKVGL_QTYPES_HPP

#include <string>
#include <cstdint>
#include <cstdio>
#include <cassert>

typedef uint32_t QHashID;

extern std::string QHashIDString( QHashID id );

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

struct QRect{
    QPosition p1;
    QPosition p2;

    bool operator == (const QRect & b) const {
        return p1.x == b.p1.x && p1.y == b.p1.y && p2.x == b.p2.x && p2.y == b.p2.y;
    }
} ;



#endif //QUICKVGL_QTYPES_HPP
