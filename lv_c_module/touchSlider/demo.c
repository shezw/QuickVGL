//
// Created by Sprite on 2023/4/3.
//
#include "lv_touchSlider.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/fb.h>
#include <linux/input.h>

#include "lvgl.h"
#include "lvgl/src/draw/lv_img_cache.h"
#include "lvgl/src/draw/lv_img_cache_builtin.h"

#include "indev/evdev.h"
#include "draw/sw/lv_draw_sw.h"

/*
    Linux frame buffer like /dev/fb0
*/

/*******************
*       DEFINE
********************/

#define DEMO_MAX 9
#define LV_HOR_RES_MAX 1280
#define LV_VER_RES_MAX 800

/* error handler */
#define handle_error(msg) do {perror(msg);exit(-1);} \
    while(0)

#define DEFAULT_LINUX_FB_PATH "/dev/fb0"
#define DEFAULT_LINUX_TOUCHPAD_PATH "/dev/input/event0"
//#define DEFAULT_LINUX_TOUCHPAD_PATH "/dev/input/mouse0"

/*
* Input system read mode.
* 0     query
* 1     sleep
* 2     poll
* 3     async
*/
#define INPUT_READ_MODE 3

#ifndef INPUT_READ_MODE
#define INPUT_READ_MODE 1
#endif

/* Sleep mode Do nothing */
#if (INPUT_READ_MODE == 2)
#include <poll.h>
#define INPUT_SAMEPLING_TIME 1
#define SYSTEM_RESPONSE_TIME 4

/*
*  SIGNAL defines in "include\uapi\asm-generic\signal.h"
*/
#elif (INPUT_READ_MODE == 3)
#include <signal.h>
#endif


//#define DISP_BUF_SIZE LV_HOR_RES_MAX * LV_VER_RES_MAX/10   /* LVGL DISP_BUF_SIZE */
#define DISP_BUF_SIZE LV_HOR_RES_MAX * LV_VER_RES_MAX   /* LVGL DISP_BUF_SIZE */

#ifndef SYSTEM_RESPONSE_TIME
#define SYSTEM_RESPONSE_TIME 5  /* Default to 5 milliseconds to keep the system responsive */
#endif

static pthread_t pt;
void *tick_thread(void * data);

void *tick_thread(void * data)
{
    (void)data;
    struct timeval last,new;
    gettimeofday(&last, NULL);
    while(1) {
        usleep(5000);
        lv_tick_inc(5); /*Tell LittelvGL that 1 milliseconds were elapsed*/
    }

    return 0;
}


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


/*******************
*       GLOBAL
********************/
static fbdev_struct fbdev_info;  /* framebuffer */
struct fb_fix_screeninfo finfo;

static screen_struct screen_info;    /* scree */
static indev_struct indev_info;  /* touchpad data */
#if (INPUT_READ_MODE==3)
int flags;
#endif


/*******************
*     PROTOTYPE
********************/
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


/**
 * Get the screen info.
 * mmap the framebuffer to memory.
 * clear the screen.
 * @param
 * @return
 */
void my_fb_init(void)
{
    fbdev_info.fd_fb = open(DEFAULT_LINUX_FB_PATH, O_RDWR);
    if(fbdev_info.fd_fb < 0)
    {
        handle_error("can not open framebuffer");
    }
    printf("Successfully opened framebuffer.\n");
    /* already get fd_fb */
    if(ioctl(fbdev_info.fd_fb, FBIOGET_VSCREENINFO, &fbdev_info.fb_var) < 0)
    {
        handle_error("can not ioctl");
    }
    if (ioctl(fbdev_info.fd_fb,FBIOGET_FSCREENINFO, &finfo))
    {
        handle_error("Error reading fixed information/n");
    }
    /* already get the var screen info */
    screen_info.width = fbdev_info.fb_var.xres;
    screen_info.height = fbdev_info.fb_var.yres;
    screen_info.bpp = fbdev_info.fb_var.bits_per_pixel;
    screen_info.line_width = fbdev_info.fb_var.xres * fbdev_info.fb_var.bits_per_pixel / 8;
    screen_info.pixel_width = fbdev_info.fb_var.bits_per_pixel / 8;
    screen_info.screen_size = fbdev_info.fb_var.xres * fbdev_info.fb_var.yres * fbdev_info.fb_var.bits_per_pixel / 8;

    printf("screen info:\n Resolution:\t%dx%d\n Bits per pixel:\t%d\n",
           screen_info.width,screen_info.height,screen_info.bpp);
    printf("frame buffer size:%d\n", finfo.smem_len);

    /* mmap the fb_base */
    fbdev_info.fb_base = (unsigned char *)mmap(NULL, screen_info.screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbdev_info.fd_fb, 0);
    if(fbdev_info.fb_base == (unsigned char *) -1)
    {
        handle_error("can not mmap frame buffer");
    }
    /* alreay get the start addr of framebuffer */
    printf("Successfully get the start address of framebuffer.\n");
    memset(fbdev_info.fb_base, 0xff, screen_info.screen_size); /* clear the screen */
    printf("Successfully clear the screen.\n");


    if (ioctl(fbdev_info.fd_fb, FBIOPAN_DISPLAY, &fbdev_info.fb_var) < 0) {
        fprintf(stderr, "active fb swap failed\n");
    }

}

/**
 * Just initialize the touchpad
 * @param
 * @return
 */
void my_touchpad_init(void)
{
#if INPUT_READ_MODE == 0
    indev_info.tp_fd = open(DEFAULT_LINUX_TOUCHPAD_PATH, O_RDWR | O_NDELAY | O_NONBLOCK); /* Query mode */
    printf("Successfully open device in nonblock mode.\n");

#else
    indev_info.tp_fd = open(DEFAULT_LINUX_TOUCHPAD_PATH, O_RDWR);  /* Other mode */
    printf("Successfully open device in normal mode.\n");

#endif


    if(indev_info.tp_fd < 0)    /* Returns -1 on error */
    {
        handle_error("can not open indev");
    }

#if (INPUT_READ_MODE == 2)  /* poll mode */
    indev_info.nfds = 1;
    indev_info.mpollfd[0].fd = indev_info.tp_fd;
    indev_info.mpollfd[0].events = POLLIN;
    indev_info.mpollfd[0].revents = 0;
    printf("Successfully run in poll mode.\n");
#elif (INPUT_READ_MODE == 3)
    signal(SIGIO, my_touchpad_sig_handler);
    fcntl(indev_info.tp_fd, F_SETOWN, getpid());
    flags = fcntl(indev_info.tp_fd, F_GETFL);
    fcntl(indev_info.tp_fd, F_SETFL, flags | FASYNC | O_NONBLOCK);
    printf("Successfully run in async mode.\n");

#endif

}

#if (INPUT_READ_MODE==0 || INPUT_READ_MODE == 1)
void my_toupad_default_handler(lv_task_t *task)
{
#if (INPUT_READ_MODE == 0 )
    while(read(indev_info.tp_fd, &indev_info.indev_event,
               sizeof(struct input_event)) > 0)
#else
	if(read(indev_info.tp_fd, &indev_info.indev_event,
				   sizeof(struct input_event)) > 0)
#endif

    {
        my_touchpad_probe_event();
    }
}
#endif /* my_toupad_default_handler */

#if (INPUT_READ_MODE==3)
/**
 * async signal handler
 * @param
 * @return
 */
void my_touchpad_sig_handler(int signal)
{
    while(read(indev_info.tp_fd, &indev_info.indev_event, sizeof(struct input_event)) > 0)
        my_touchpad_probe_event();
}
#endif  /* my_touchpad_sig_handler */


#if (INPUT_READ_MODE==2)
/**
 * A thread to collect input data of screen.
 * @param
 * @return
 */
void my_touchpad_poll_handler(lv_task_t *task)
{
    (void)task;
    int len;
    len = poll(indev_info.mpollfd, indev_info.nfds, INPUT_SAMEPLING_TIME);
    if(len > 0)         /* There is data to read */
    {
        if(read(indev_info.tp_fd, &indev_info.indev_event,
                sizeof(indev_info.indev_event)) > 0)
        {
            my_touchpad_probe_event();
        }

    }
    else if(len == 0)     /* Time out */
    {
        /* Do nothing */
    }
    else     /* Error */
    {
        handle_error("poll error!");
    }
touchdown_err:      /* Do nothing. Just return and ready for next event come. */
    return;
}
#endif


void my_touchpad_probe_event(void)
{

    switch(indev_info.indev_event.type)
    {
        case EV_KEY:    /* Key event. Provide the pressure data of touchscreen*/
            if(indev_info.indev_event.code == BTN_TOUCH)          /* Screen touch event */
            {
                if(1 == indev_info.indev_event.value)         /* Touch down */
                {
                    indev_info.touchdown = true;
                }
                else if(0 == indev_info.indev_event.value)     /* Touch up */
                {
                    indev_info.touchdown = false;
                }
                else                            /* Unexcepted data */
                {
                    goto touchdown_err;
                }
            }
            break;
        case EV_ABS:    /* Abs event. Provide the position data of touchscreen*/
            if(indev_info.indev_event.code == ABS_MT_POSITION_X)
            {
                indev_info.last_x = indev_info.indev_event.value;
            }
            if(indev_info.indev_event.code == ABS_MT_POSITION_Y)
            {
                indev_info.last_y = indev_info.indev_event.value;
            }
            break;
        default:
            break;
    }
    touchdown_err:      /* Do nothing. Just return and ready for next event come. */
    return;

}


/**
 * releated to disp_drv.flush_cb
 * @param disp
 * @param area
 * @param color_p
 * @return
 */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++)
    {
        for(x = area->x1; x <= area->x2; x++)
        {
            memcpy(fbdev_info.fb_base + x * screen_info.pixel_width + y * screen_info.line_width,
                   &color_p->full, sizeof(lv_color_t));
            color_p++;
        }
    }

    lv_disp_flush_ready(disp);
}

/**
 * releated to indev_drv.readcb
 * @param indev
 * @param data
 * @return false
 */
void my_input_read(lv_indev_t * indev, lv_indev_data_t*data)
{
    printf("my_input_read %d %d\n",indev->btn_points[0].x,indev->btn_points[0].y);
}

void swipeStartContainer2( lv_event_t * evt ){

    QVGLC_touchSliderTouchStart( 0, evt );
}

void swipeMoveContainer2( lv_event_t * evt ){

    QVGLC_touchSliderTouchMove( 0, evt );
}

void swipeEndContainer2( lv_event_t * evt ){

    QVGLC_touchSliderTouchUp( 0, evt );
}


static char * pngIcons[DEMO_MAX] = {
    "S:/customer/res/60889_flip/images/mode_boiled.png",
    "S:/customer/res/60889_flip/images/mode_normal.png",
    "S:/customer/res/60889_flip/images/mode_warm.png",
    "S:/customer/res/60889_flip/images/mode_honey.png",
    "S:/customer/res/60889_flip/images/mode_tea.png",
    "S:/customer/res/60889_flip/images/mode_boiled.png",
    "S:/customer/res/60889_flip/images/mode_normal.png",
    "S:/customer/res/60889_flip/images/mode_warm.png",
    "S:/customer/res/60889_flip/images/mode_honey.png",
};

struct TouchSliderElement_t{

    lv_obj_t * icon;
    lv_obj_t * bg;
    lv_label_t * label;

};

void elementUpdatedCall( lv_obj_t * element, int idx, int selected ){

    if (idx==selected) {
        lv_obj_set_style_text_color(lv_obj_get_child(element,0), lv_color_hex(0xff3366ff), 0);
    }else{
        lv_obj_set_style_text_color(lv_obj_get_child(element,0), lv_color_hex(0xff555555), 0);
    }
}

void testImageSlider(){

    int max_image = 99;

    lv_obj_t * container2 = lv_obj_create(lv_scr_act());

    lv_obj_set_width(container2,800);
    lv_obj_set_height(container2,1280);
    lv_obj_set_style_bg_color(container2,lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_scrollbar_mode(container2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(container2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * image2 = lv_img_create(container2);
//    lv_img_set_src(image2,&bigicon);
    lv_img_set_src(image2,"S:/customer/res/60889_flip/images/background.png");

    lv_obj_set_style_pad_left(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_x(container2,0);

    lv_obj_t ** elements2 = (lv_obj_t**) malloc(max_image*sizeof (lv_obj_t*));

    for (int i = 0; i < max_image; i++) {

        elements2[i] = lv_obj_create( container2 );
        lv_obj_set_width(elements2[i],256);
        lv_obj_set_height(elements2[i],278);

        lv_obj_set_style_bg_color(elements2[i], lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(elements2[i], 0x0 ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(elements2[i], 0, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_x(elements2[i],260);
//        lv_obj_set_y(elements2[i],50);

//        lv_obj_t * txt = lv_label_create( elements2[i] );
//        lv_label_set_text_fmt(txt, "%d", i);
//        lv_obj_set_style_text_color( txt, lv_color_hex(0xff888888), 0 );

//        printf("PNG src %s\n",pngIcons[i]);

        lv_obj_t * img = lv_img_create( elements2[i] );
        lv_img_set_src( img, pngIcons[i%9] );
        lv_obj_set_scrollbar_mode(elements2[i], LV_SCROLLBAR_MODE_OFF);
        lv_obj_clear_flag(elements2[i], LV_OBJ_FLAG_SCROLLABLE);
        lv_img_set_antialias(img, false); // 移除抗锯齿

//        lv_obj_set_style_bg_img_src( elements2[i], &bigicon, LV_PART_MAIN | LV_STATE_DEFAULT );
//        lv_obj_set_style_bg_img_src( elements2[i], pngIcons[i], LV_PART_MAIN | LV_STATE_DEFAULT );
    }
    printf("Element created\n");

    QVGLC_TouchSliderConfig_t config2 = {
        .fx             = TOUCH_SLIDER_FX_MODE_CIRCLE,
        .direction      = TOUCH_SLIDER_DIRECTION_Y,
//        .direction      = TOUCH_SLIDER_DIRECTION_X,
//        .layout         = TOUCH_SLIDER_LAYOUT_LT,
        .layout         = TOUCH_SLIDER_LAYOUT_CENTER,
//        .layout         = TOUCH_SLIDER_LAYOUT_RB,
        .inertia        = 0.8,
        .attenuation    = 2.5,
        .canLoop        = true,
//    .canLoop        = false,
//        .useScale       = true,
//        .useScale       = false,
        .useOpacity     = true,
        .gapSize        = 256,
        .gapRatio       = 0.95,
        .containerSize  = 1280,
        .objectWidth    = 256,
        .objectHeight   = 278,
        .index          = 5,
        .maxBrother     = 9,
        .radius         = 3200
    };

    int fd2 = QVGLC_touchSliderInit( elements2, max_image, &config2 );

    lv_obj_add_event_cb( container2, swipeStartContainer2, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb( container2, swipeMoveContainer2, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb( container2, swipeEndContainer2, LV_EVENT_RELEASED, NULL);

    QVGLC_touchSliderRegisterElementUpdate( fd2, elementUpdatedCall );


}

void testLabelSlider(){

    int max_label = 299;

    lv_obj_t * container2 = lv_obj_create(lv_scr_act());

    lv_obj_set_width(container2,800);
    lv_obj_set_height(container2,1280);
    lv_obj_set_style_bg_color(container2,lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_scrollbar_mode(container2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(container2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * image2 = lv_img_create(container2);
//    lv_img_set_src(image2,&bigicon);
    lv_img_set_src(image2,"S:/customer/res/60889_flip/images/background.png");

    lv_obj_set_style_pad_left(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_y(container2,0);



    lv_obj_t ** elements2 = (lv_obj_t**) malloc(max_label*sizeof (lv_obj_t*));

    for (int i = 0; i < max_label; i++) {

        elements2[i] = lv_obj_create( container2 );
        lv_obj_set_width(elements2[i],120);
        lv_obj_set_height(elements2[i],120);

        lv_obj_set_style_bg_color(elements2[i], lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(elements2[i], 0x0 ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(elements2[i], 0, LV_PART_MAIN | LV_STATE_DEFAULT);

//        lv_obj_set_x(elements2[i],260);
        lv_obj_set_y(elements2[i],50);

        lv_obj_t * txt = lv_label_create( elements2[i] );
        lv_label_set_text_fmt(txt, "%d", i);
        lv_obj_set_style_text_color( txt, lv_color_hex(0xff555555), 0 );

    }
    printf("Element created\n");

    QVGLC_TouchSliderConfig_t config2 = {
            .direction      = TOUCH_SLIDER_DIRECTION_Y,
//            .direction      = TOUCH_SLIDER_DIRECTION_X,
//            .layout         = TOUCH_SLIDER_LAYOUT_LT,
            .layout         = TOUCH_SLIDER_LAYOUT_CENTER,
//            .layout         = TOUCH_SLIDER_LAYOUT_RB,
            .inertia        = 0.8,
            .attenuation    = 2.5,
            .canLoop        = true,
//        .canLoop        = false,
//            .useScale       = true,
        .useScale       = false,
            .useOpacity     = true,
            .gapSize        = 120,
            .containerSize  = 1280,
            .objectWidth    = 120,
            .objectHeight   = 120,
            .index          = 0,
            .maxBrother     = 3,
    };

    int fd2 = QVGLC_touchSliderInit( elements2, max_label, &config2 );

    lv_obj_add_event_cb( container2, swipeStartContainer2, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb( container2, swipeMoveContainer2, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb( container2, swipeEndContainer2, LV_EVENT_RELEASED, NULL);

    QVGLC_touchSliderRegisterElementUpdate( fd2, elementUpdatedCall );


}



void testRadiusLabelSlider(){

    int max_label = 299;

    lv_obj_t * container2 = lv_obj_create(lv_scr_act());

    lv_obj_set_width(container2,800);
    lv_obj_set_height(container2,1280);
    lv_obj_set_style_bg_color(container2,lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_scrollbar_mode(container2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(container2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * image2 = lv_img_create(container2);
//    lv_img_set_src(image2,&bigicon);
    lv_img_set_src(image2,"S:/customer/res/60889_flip/images/background.png");

    lv_obj_set_style_pad_left(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_y(container2,0);



    lv_obj_t ** elements2 = (lv_obj_t**) malloc(max_label*sizeof (lv_obj_t*));

    for (int i = 0; i < max_label; i++) {

        elements2[i] = lv_obj_create( container2 );
        lv_obj_set_width(elements2[i],120);
        lv_obj_set_height(elements2[i],120);

        lv_obj_set_style_bg_color(elements2[i], lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(elements2[i], 0x0 ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(elements2[i], 0, LV_PART_MAIN | LV_STATE_DEFAULT);

//        lv_obj_set_x(elements2[i],260);
        lv_obj_set_y(elements2[i],50);

        lv_obj_t * txt = lv_label_create( elements2[i] );
        lv_label_set_text_fmt(txt, "%d", i);
        lv_obj_set_style_text_color( txt, lv_color_hex(0xff555555), 0 );

    }
    printf("Element created\n");

    QVGLC_TouchSliderConfig_t config2 = {
            .fx             = TOUCH_SLIDER_FX_MODE_CIRCLE,
            .direction      = TOUCH_SLIDER_DIRECTION_Y,
//            .direction      = TOUCH_SLIDER_DIRECTION_X,
//            .layout         = TOUCH_SLIDER_LAYOUT_LT,
            .layout         = TOUCH_SLIDER_LAYOUT_CENTER,
//            .layout         = TOUCH_SLIDER_LAYOUT_RB,
            .inertia        = 0.8f,
            .attenuation    = 2.5f,
            .canLoop        = true,
//        .canLoop        = false,
//            .useScale       = true,
            .useScale       = false,
            .useOpacity     = true,
            .gapSize        = 60,
//            .gapRatio       = 0.8f,
            .containerSize  = 1280,
            .objectWidth    = 120,
            .objectHeight   = 120,
            .index          = 0,
            .maxBrother     = 15,
            .springEdgeRatio= 0.8f,
            .radius         = 300
    };

    int fd2 = QVGLC_touchSliderInit( elements2, max_label, &config2 );

    lv_obj_add_event_cb( container2, swipeStartContainer2, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb( container2, swipeMoveContainer2, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb( container2, swipeEndContainer2, LV_EVENT_RELEASED, NULL);

    QVGLC_touchSliderRegisterElementUpdate( fd2, elementUpdatedCall );


}


void testVerticalRadiusLabelSlider(){

    int max_label = 299;

    lv_obj_t * container2 = lv_obj_create(lv_scr_act());

    lv_obj_set_width(container2,800);
    lv_obj_set_height(container2,1280);
    lv_obj_set_style_bg_color(container2,lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_scrollbar_mode(container2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(container2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * image2 = lv_img_create(container2);
//    lv_img_set_src(image2,&bigicon);
    lv_img_set_src(image2,"S:/customer/res/60889_flip/images/background.png");

    lv_obj_set_style_pad_left(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_y(container2,0);



    lv_obj_t ** elements2 = (lv_obj_t**) malloc(max_label*sizeof (lv_obj_t*));

    for (int i = 0; i < max_label; i++) {

        elements2[i] = lv_obj_create( container2 );
        lv_obj_set_width(elements2[i],120);
        lv_obj_set_height(elements2[i],120);

        lv_obj_set_style_bg_color(elements2[i], lv_color_hex(0x0) ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(elements2[i], 0x0 ,LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(elements2[i], 0, LV_PART_MAIN | LV_STATE_DEFAULT);

//        lv_obj_set_x(elements2[i],260);
        lv_obj_set_y(elements2[i],50);

        lv_obj_t * txt = lv_label_create( elements2[i] );
        lv_label_set_text_fmt(txt, "%d", i);
        lv_obj_set_style_text_color( txt, lv_color_hex(0xff555555), 0 );

    }
    printf("Element created\n");

    QVGLC_TouchSliderConfig_t config2 = {
//            .fx             = TOUCH_SLIDER_FX_MODE_CIRCLE,
//            .direction      = TOUCH_SLIDER_DIRECTION_Y,
            .direction      = TOUCH_SLIDER_DIRECTION_X,
//            .layout         = TOUCH_SLIDER_LAYOUT_LT,
            .layout         = TOUCH_SLIDER_LAYOUT_CENTER,
//            .layout         = TOUCH_SLIDER_LAYOUT_RB,
            .inertia        = 0.8f,
            .attenuation    = 2.5f,
            .canLoop        = true,
//        .canLoop        = false,
//            .useScale       = true,
            .useScale       = false,
            .useOpacity     = true,
            .gapSize        = 80,
//            .gapRatio       = 0.8,
            .containerSize  = 800,
            .objectWidth    = 80,
            .objectHeight   = 80,
            .index          = 0,
            .maxBrother     = 9,
            .springEdgeRatio = 0.75f,
            .radius         = 1600
    };

    int fd2 = QVGLC_touchSliderInit( elements2, max_label, &config2 );

    lv_obj_add_event_cb( container2, swipeStartContainer2, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb( container2, swipeMoveContainer2, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb( container2, swipeEndContainer2, LV_EVENT_RELEASED, NULL);

    QVGLC_touchSliderRegisterElementUpdate( fd2, elementUpdatedCall );


}


int main(){

    LV_IMG_DECLARE(bigicon);

    lv_init();
    my_fb_init();
//    my_touchpad_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = my_disp_flush;
    disp_drv.hor_res    = 800;
    disp_drv.ver_res    = 1280;
    disp_drv.draw_ctx_init = lv_draw_sw_init_ctx;
    disp_drv.draw_ctx_deinit = lv_draw_sw_deinit_ctx;
    disp_drv.draw_ctx_size = 4*1024*1024;
//    disp_drv.full_refresh = 1;
//    disp_drv.sw_rotate = 1;
//    disp_drv.rotated = LV_DISP_ROT_90;
    lv_disp_drv_register(&disp_drv);

    evdev_init("/dev/input/event0");
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);             /*Descriptor of a input device driver*/
    indev_drv.type = LV_INDEV_TYPE_POINTER; /*Touch pad is a pointer-like device*/
    indev_drv.read_cb = evdev_read;            /*Set your driver function*/

    lv_indev_t *indev    = lv_indev_drv_register(&indev_drv);
    lv_obj_t *cursor_img = lv_img_create(lv_scr_act());
    lv_img_set_src(cursor_img, LV_SYMBOL_UP);
    lv_indev_set_cursor(indev, cursor_img);

    printf("START TOUCH SLIDER DEMO\n");

    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);

//    testImageSlider();

//    testLabelSlider();

    testRadiusLabelSlider();
//    testVerticalRadiusLabelSlider();

    printf("touchSliderInit\n");

    pthread_create(&pt, NULL, tick_thread, NULL);

    static int timeCount = 0;
    static int second = 0;

    while (true){

        lv_task_handler();
        usleep(SYSTEM_RESPONSE_TIME * 1000);

//        lv_tick_inc(SYSTEM_RESPONSE_TIME);
//        sleep(1);
    }

}
