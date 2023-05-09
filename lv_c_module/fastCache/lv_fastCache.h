//
// Created by Sprite on 2023/5/2.
//

#ifndef QUICKVGL_LV_FASTCACHE_H
#define QUICKVGL_LV_FASTCACHE_H

#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

uint8_t * fastReadLVPNG( char * path, int * width, int * height );

#endif //QUICKVGL_LV_FASTCACHE_H
