//
// Created by Sprite on 2023/5/2.
//
#include <stdlib.h>
#include <png.h>
#include <zlib.h>
#include "lv_fastCache.h"

/**
 * USE LIB-PNG16
 */
#include <png.h>
#include <zlib.h>
typedef struct {
    int width, height;
    png_byte color_type;
    png_byte bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep * row_pointers;
} png_img_t;

static uint8_t * read_png_file(png_img_t * p, const char* file_name )
{
    char header[8];    // 8 is the maximum size that can be checked

    /*open file and test for it being a png*/
    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        printf("%s", "PNG file %s could not be opened for reading");
        return 0;
    }

    size_t rcnt = fread(header, 1, 8, fp);
    if (rcnt != 8 || png_sig_cmp((png_const_bytep)header, 0, 8)) {
        printf("%s is not recognized as a PNG file", file_name);
        return 0;
    }

    /*initialize stuff*/
    p->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!p->png_ptr) {
        printf("%s", "png_create_read_struct failed");
        return 0;
    }

    p->info_ptr = png_create_info_struct(p->png_ptr);
    if (!p->info_ptr) {
        printf("%s", "png_create_info_struct failed");
        return 0;
    }
    if (setjmp(png_jmpbuf(p->png_ptr))) {
        printf("%s", "Error during init_io");
        return 0;
    }
    png_init_io(p->png_ptr, fp);
    png_set_sig_bytes(p->png_ptr, 8);

    png_read_info(p->png_ptr, p->info_ptr);

    p->width = png_get_image_width(p->png_ptr, p->info_ptr);
    p->height = png_get_image_height(p->png_ptr, p->info_ptr);
    p->color_type = png_get_color_type(p->png_ptr, p->info_ptr);
    p->bit_depth = png_get_bit_depth(p->png_ptr, p->info_ptr);

    p->number_of_passes = png_set_interlace_handling(p->png_ptr);
    png_read_update_info(p->png_ptr, p->info_ptr);

    /*read file*/
    if (setjmp(png_jmpbuf(p->png_ptr))) {
        printf("%s", "Error during read_image");
        return 0;
    }
    p->row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * p->height);

    int y;
    for (y=0; y<p->height; y++)
        p->row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(p->png_ptr,p->info_ptr));

    png_read_image(p->png_ptr, p->row_pointers);

    uint8_t * buffer = (uint8_t*)malloc( p->height * p->width * 4 );
    for (int j = 0; j < p->height; j++) {
        memcpy( &buffer[j*p->width * 4], p->row_pointers[j], p->width * 4 );
    }
    fclose(fp);
    return buffer;
    return 0;
}

static void png_release(png_img_t * p)
{
    int y;
    for (y=0; y<p->height; y++) free(p->row_pointers[y]);

    free(p->row_pointers);

    png_destroy_read_struct(&p->png_ptr, &p->info_ptr, NULL);
}


uint8_t * fastReadLVPNG( char * path, int * width, int * height ) {

    char file[256];
    memset( file, 0, 256 );

    if (path[0]=='/'){
        memcpy( file, path, strlen(path) );
    }else if( path[1] == ':' ){
        file[0] = '.';
        file[1] = '/';
        memcpy( &file[2], &path[2], strlen(path)-2);
    }

    png_img_t p;
    uint8_t * img_data = read_png_file( &p, file );

    *width = p.width;
    *height= p.height;

    png_release(&p);

    return img_data;
}
