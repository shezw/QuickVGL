//
// Created by 志伟佘 on 2023/5/14.
//

#ifndef QUICKVGL_TESTHAL_H
#define QUICKVGL_TESTHAL_H

#ifndef SYSTEM_RESPONSE_TIME
#define SYSTEM_RESPONSE_TIME 5  /* Default to 5 milliseconds to keep the system responsive */
#endif

#if OS_LINUX
#include <linux/fb.h>
#include <linux/input.h>
#include "indev/evdev.h"

/*******************
*     TYPEDEF
********************/
/* Framebuffer info */
typedef struct
{
    int fd_fb;
    unsigned char *fb_base;

    struct fb_var_screeninfo fb_var;
} fbdev_struct;

/* Lcd info */
typedef struct
{
    int width;
    int height;
    int screen_size;
    int line_width;
    int bpp;
    int pixel_width;
} screen_struct;


/* Input device */
typedef struct
{
    /*  */
    int tp_fd;

#ifndef DEFAULT_LINUX_TOUCHPAD_PATH
    char *event;
#endif

    /* Data of touch panel */
    bool touchdown;
    unsigned short last_x;  /* Abs_mt_x */
    unsigned short last_y;  /* Abs_mt_y */

#if (INPUT_READ_MODE == 0)
    /* query mode */

#elif (INPUT_READ_MODE == 2)
    #define DEVICE_NUM 1
    /* poll mode */
    nfds_t nfds;
    struct pollfd mpollfd[DEVICE_NUM];


#elif (INPUT_READ_MODE == 3)
    /* signal mode */

#endif

    struct input_event indev_event;
} indev_struct;


void my_fb_init(void);
void my_touchpad_init(void);

#if (INPUT_READ_MODE==0 || INPUT_READ_MODE == 1)
void my_toupad_default_handler(lv_task_t *task);
#endif /* my_toupad_default_handler */

#if (INPUT_READ_MODE==2)
void my_touchpad_poll_handler(lv_task_t *task);
#endif

#if (INPUT_READ_MODE==3)
void my_touchpad_sig_handler(int signal);
#endif

void my_touchpad_probe_event(void);
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
bool my_touchpad_read(lv_indev_drv_t *indev, lv_indev_data_t *data);

#define DISP_BUF_SIZE LV_HOR_RES_MAX * LV_VER_RES_MAX   /* LVGL DISP_BUF_SIZE */

#endif

void testHalInit();

void testHalLoop();

#endif //QUICKVGL_TESTHAL_H
