//
// Created by Sprite on 2023/4/3.
//

#include <stdio.h>
#include "lv_touchSlider.h"

#define DEMO_MAX 10


#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/fb.h>
#include   <pthread.h>
#include <sys/epoll.h>

#ifndef FBDEV_PATH
#define FBDEV_PATH  "/dev/fb0"
#endif

#if USE_BSD_FBDEV
static struct bsd_fb_var_info vinfo;
static struct bsd_fb_fix_info finfo;
#else
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
#endif /* USE_BSD_FBDEV */
static char *fbp = 0,*fbpbase = 0;
static long int screensize = 0;
static long int half_screensize = 0;
static int fbfd = 0;

#define DISP_BUF_SIZE LV_HOR_RES_MAX*LV_VER_RES_MAX*4
#define LV_HOR_RES_MAX 1024
#define LV_VER_RES_MAX 600

char toggle = 0;
/**
 * Flush a buffer to the marked area
 * @param drv pointer to driver where this function belongs
 * @param area an area where to copy `color_p`
 * @param color_p an array of pixels to copy to the `area` part of the screen
 */


void fbdev_init(void)
{
#ifdef USE_SSD20X
    init_ssd20x_disp();

#else
    // Open the file for reading and writing
    fbfd = open(FBDEV_PATH, O_RDWR);
    if(fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        return;
    }
    LV_LOG_INFO("The framebuffer device was opened successfully");

    // Make sure that the display is on.
    if (ioctl(fbfd, FBIOBLANK, FB_BLANK_UNBLANK) != 0) {
        perror("ioctl(FBIOBLANK)");
        return;
    }

#if USE_BSD_FBDEV
    struct fbtype fb;
    unsigned line_length;

    //Get fb type
    if (ioctl(fbfd, FBIOGTYPE, &fb) != 0) {
        perror("ioctl(FBIOGTYPE)");
        return;
    }

    //Get screen width
    if (ioctl(fbfd, FBIO_GETLINEWIDTH, &line_length) != 0) {
        perror("ioctl(FBIO_GETLINEWIDTH)");
        return;
    }

    vinfo.xres = (unsigned) fb.fb_width;
    vinfo.yres = (unsigned) fb.fb_height;
    vinfo.bits_per_pixel = fb.fb_depth;
    vinfo.xoffset = 0;
    vinfo.yoffset = 0;
    finfo.line_length = line_length;
    finfo.smem_len = finfo.line_length * vinfo.yres;
#else /* USE_BSD_FBDEV */

    // Get fixed screen information
    if(ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        return;
    }

    // Get variable screen information
    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        return;
    }
#endif /* USE_BSD_FBDEV */

    LV_LOG_INFO("%dx%d, %dbpp", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // Figure out the size of the screen in bytes
    screensize =  finfo.smem_len; //finfo.line_length * vinfo.yres;
    half_screensize = screensize/2;
    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    fbpbase =  fbp;
    if((intptr_t)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        return;
    }
    memset(fbp, 0, screensize);

    LV_LOG_INFO("The framebuffer device was mapped to memory successfully");
#endif
}

void fbdev_exit(void)
{
    close(fbfd);
}


void fbdev_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p)
{

    if(fbp == NULL ||
       area->x2 < 0 ||
       area->y2 < 0 ||
                  area->x1 > (int32_t)vinfo.xres - 1 ||
                                      area->y1 > (int32_t)vinfo.yres - 1) {
        lv_disp_flush_ready(drv);


        return;
    }


#if LV_DUAL_FRAMEBUFFER
    if(toggle == 0)
    {
        // printf("toggle set to 1 \r\n");
        toggle = 1;
        vinfo.yoffset = 0;
        fbp =fbpbase;
    }
    else
    {
        // printf("toggle set to 0 \r\n");
        toggle = 0;
        vinfo.yoffset = vinfo.yres;
        fbp =  fbpbase + half_screensize;
    }
#else
    fbp = fbpbase;
#endif

    /*Truncate the area to the screen*/
    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2 > (int32_t)vinfo.xres - 1 ? (int32_t)vinfo.xres - 1 : area->x2;
    int32_t act_y2 = area->y2 > (int32_t)vinfo.yres - 1 ? (int32_t)vinfo.yres - 1 : area->y2;


    lv_coord_t w = (act_x2 - act_x1 + 1);
    long int location = 0;
    long int byte_location = 0;
    unsigned char bit_location = 0;

    /*32 or 24 bit per pixel*/
    if(vinfo.bits_per_pixel == 32 || vinfo.bits_per_pixel == 24) {

        uint32_t * fbp32 = (uint32_t *)fbp;
        int32_t y;
        for(y = act_y1; y <= act_y2; y++) {
#if(LV_DUAL_FRAMEBUFFER == 1)
            location = (act_x1 + vinfo.xoffset) + (y) * finfo.line_length / 4;
#else
            location = (act_x1 + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length / 4;//双buffer yoffset会coredump越界
#endif
            memcpy(&fbp32[location], (uint32_t *)color_p, (act_x2 - act_x1 + 1) * 4);
            color_p += w;
        }
    }
    else {
        /*Not supported bit per pixel*/
    }


    //May be some direct update command is required
    //ret = ioctl(state->fd, FBIO_UPDATE, (unsigned long)((uintptr_t)rect));

#if LV_DUAL_FRAMEBUFFER
    // printf("vinfo.yoffset = %d \r\n ",vinfo.yoffset);
	if (ioctl(fbfd, FBIOPAN_DISPLAY, &vinfo) < 0) {
		fprintf(stderr, "active fb swap failed\n");
	}

#endif

    lv_disp_flush_ready(drv);
}

void demo_init_fb(){

    /*Linux frame buffer device init*/
    fbdev_init();

    static lv_color_t buf[DISP_BUF_SIZE];
    static lv_color_t buf1x[DISP_BUF_SIZE];
    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, buf1x, DISP_BUF_SIZE);



    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = LV_HOR_RES_MAX;
    disp_drv.ver_res    = LV_VER_RES_MAX;

    lv_disp_drv_register(&disp_drv);

}

int main(){

    lv_init();
    demo_init_fb();

    printf("START TOUCH SLIDER DEMO\n");

    lv_obj_t * main = lv_obj_create(lv_scr_act());
    lv_obj_t * elements[DEMO_MAX];

    for (int i = 0; i < DEMO_MAX; ++i) {

        elements[i] = lv_obj_create( main );
    }
    printf("Element created\n");

    TouchSliderConfig_t config = {
        .direction      = TOUCH_SLIDER_DIRECTION_X,
        .inertia        = 0.8,
        .attenuation    = 2.5,
        .canLoop        = true,
        .gapSize        = 100,
        .index          = 0,
        .maxBrother     = 2,
        .springEdge     = false
    };

    int fd = touchSliderInit( elements, DEMO_MAX, &config );

    printf("touchSliderInit\n");



    printf("TOUCH SLIDER(%d) DEMO DONE\n", fd );

}
