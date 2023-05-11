//
// Created by Sprite on 2023/4/3.
//

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


static lv_obj_t * chart1;
static lv_chart_series_t * ser1;
static lv_chart_series_t * ser2;

static void draw_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);

    /*Add the faded area before the lines are drawn*/
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(dsc->part == LV_PART_ITEMS) {
        if(!dsc->p1 || !dsc->p2) return;

        /*Add a line mask that keeps the area below the line*/
        lv_draw_mask_line_param_t line_mask_param;
        lv_draw_mask_line_points_init(&line_mask_param, dsc->p1->x, dsc->p1->y, dsc->p2->x, dsc->p2->y,
                                      LV_DRAW_MASK_LINE_SIDE_BOTTOM);
        int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);

        /*Add a fade effect: transparent bottom covering top*/
        lv_coord_t h = lv_obj_get_height(obj);
        lv_draw_mask_fade_param_t fade_mask_param;
        lv_draw_mask_fade_init(&fade_mask_param, &obj->coords, LV_OPA_COVER, obj->coords.y1 + h / 8, LV_OPA_TRANSP,
                               obj->coords.y2);
        int16_t fade_mask_id = lv_draw_mask_add(&fade_mask_param, NULL);

        /*Draw a rectangle that will be affected by the mask*/
        lv_draw_rect_dsc_t draw_rect_dsc;
        lv_draw_rect_dsc_init(&draw_rect_dsc);
        draw_rect_dsc.bg_opa = LV_OPA_40;
        draw_rect_dsc.bg_color = dsc->line_dsc->color;

        lv_area_t a;
        a.x1 = dsc->p1->x;
        a.x2 = dsc->p2->x - 1;
        a.y1 = LV_MIN(dsc->p1->y, dsc->p2->y);
        a.y2 = obj->coords.y2;
        lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);

        /*Remove the masks*/
        lv_draw_mask_free_param(&line_mask_param);
        lv_draw_mask_free_param(&fade_mask_param);
        lv_draw_mask_remove_id(line_mask_id);
        lv_draw_mask_remove_id(fade_mask_id);
    }
        /*Hook the division lines too*/
    else if(dsc->part == LV_PART_MAIN) {
        if(dsc->line_dsc == NULL || dsc->p1 == NULL || dsc->p2 == NULL) return;

        /*Vertical line*/
        if(dsc->p1->x == dsc->p2->x) {
            dsc->line_dsc->color  = lv_palette_lighten(LV_PALETTE_GREY, 1);
            if(dsc->id == 3) {
                dsc->line_dsc->width  = 2;
                dsc->line_dsc->dash_gap  = 0;
                dsc->line_dsc->dash_width  = 0;
            }
            else {
                dsc->line_dsc->width = 1;
                dsc->line_dsc->dash_gap  = 6;
                dsc->line_dsc->dash_width  = 6;
            }
        }
            /*Horizontal line*/
        else {
            if(dsc->id == 2) {
                dsc->line_dsc->width  = 2;
                dsc->line_dsc->dash_gap  = 0;
                dsc->line_dsc->dash_width  = 0;
            }
            else {
                dsc->line_dsc->width = 2;
                dsc->line_dsc->dash_gap  = 6;
                dsc->line_dsc->dash_width  = 6;
            }

            if(dsc->id == 1  || dsc->id == 3) {
                dsc->line_dsc->color  = lv_palette_main(LV_PALETTE_GREEN);
            }
            else {
                dsc->line_dsc->color  = lv_palette_lighten(LV_PALETTE_GREY, 1);
            }
        }
    }
}
static void add_data(lv_timer_t * timer)
{
//    LV_UNUSED(timer);
//    static uint32_t cnt = 0;
////    lv_chart_set_next_value(chart1, ser1, lv_rand(20, 90));
//
//    if(cnt % 4 == 0) lv_chart_set_next_value(chart1, ser2, lv_rand(40, 60));
//
//    cnt++;
}

/**
 * Add a faded area effect to the line chart and make some division lines ticker
 */
void lv_example_chart_2(void)
{
    /*Create a chart1*/
    chart1 = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart1, 480, 320);
    lv_obj_set_style_bg_color(chart1, lv_color_black(), LV_PART_MAIN);
    lv_obj_center(chart1);
    lv_chart_set_type(chart1, LV_CHART_TYPE_SCATTER);   /*Show lines and points too*/

    lv_chart_set_div_line_count(chart1, 8, 8);

    lv_chart_set_range(chart1, LV_CHART_AXIS_PRIMARY_Y, 0, 800);
    lv_chart_set_range(chart1, LV_CHART_AXIS_PRIMARY_X, 0, 480);
    lv_chart_set_point_count(chart1, 480);

//    lv_table_set_col_cnt( chart , 2400);
    lv_obj_add_event_cb(chart1, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_chart_set_update_mode(chart1, LV_CHART_UPDATE_MODE_CIRCULAR);

//    lv_chart_set_range(chart1, LV_CHART_AXIS_PRIMARY_Y, 0, 1000);
//    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, points);

    /*Add two data series*/
//    ser1 = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    ser2 = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_SECONDARY_Y);

    uint32_t i;
    for(i = 0; i < 480; i++) {

        int yMax = lv_bezier3(i, 0, 600, 220, 570);
        lv_chart_set_value_by_id2(chart1, ser2, i, i, yMax);

//        int yMax = lv_bezier3(i, 0, 160, 22, 390);
//        lv_chart_set_next_value(chart1, ser2, yMax);

//        lv_chart_set_next_value(chart1, ser1, lv_rand(20, 90));
//        lv_chart_set_next_value(chart1, ser2, lv_rand(30, 70));
    }

//    lv_timer_create(add_data, 200, NULL);
}




int main(){

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

    printf("START CURVE DEMO\n");



    if(0){

        lv_example_chart_2();
    }else {

        int pts[16] = {0, 90, 300, 500, 550, 600, 321, 400, 200, 115, 113, 115, 0, 660, 45, 50};

/*Create an array for the points of the line*/
//    lv_point_t line_points[15] = {0};

//    for (int idx=0;idx<15;idx++) {
//        line_points[idx].x = idx * 60;
//        line_points[idx].y = pts[idx];
//    }

        size_t points = 1600;
//    int firstP = 0;
//
//    lv_point_t * pointList = (lv_point_t * )malloc( points * sizeof (struct lv_point_t *));
//
//    // 3阶
//    for (int i = 0; i < points; i++) {
//
//        if (i>=742 && i <=746) {
//            firstP = 12;
//
//        }else if ( i < ( points/15 ) ){
//
//            firstP = 0;
//
//        }else if( i >= ( points - points/15 ) ){
//
//            firstP = 15-1-3;
//
//        }else{
//
//            firstP = i/(points/15) - 1;
//        }
//
//        pointList[i].x = i;
//
//        pointList[i].y = lv_bezier3(i, pts[firstP], pts[firstP+1], pts[firstP+2], LV_BEZIER_VAL_MAX);
//        printf("P %d, %4d, %4d, %4d    --  %d \n ",i, pts[firstP], pts[firstP+1], pts[firstP+2],pointList[i].y);
//
//    }

        lv_obj_t *chart = lv_chart_create(lv_scr_act());
//    lv_obj_align(chart,LV_ALIGN_DEFAULT,50,90);
        lv_obj_set_size(chart, 800, 480);
        lv_obj_set_style_line_color(chart, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);
        lv_obj_set_style_line_opa(chart, LV_OPA_80, LV_PART_MAIN);
        lv_obj_set_style_pad_all(chart, 0, LV_PART_MAIN);
//    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(chart, lv_color_black(), LV_PART_MAIN);
        lv_obj_set_style_border_width(chart, 0, LV_PART_MAIN);
        lv_chart_set_type(chart, LV_CHART_TYPE_SCATTER);
        lv_chart_set_div_line_count(chart, 16, 12);
        lv_chart_series_t *series = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_AMBER),
                                                        LV_CHART_AXIS_PRIMARY_Y);
        lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, points);
        lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, points);
        lv_chart_set_point_count(chart, 800);

        lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

        /*Add a faded are effect*/

        for (int i = 0; i <= points; i++) {
            int yMax = lv_bezier3(i, 300, 1600, 120, LV_BEZIER_VAL_MAX);
            lv_chart_set_value_by_id2(chart, series, i, i, yMax);
        }

        lv_chart_refresh(chart);


//    /*Create style*/
//    static lv_style_t style_line;
//    lv_style_init(&style_line);
//    lv_style_set_line_width(&style_line, 8);
//    lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_BLUE));
//    lv_style_set_line_rounded(&style_line, true);
//
//    /*Create a line and apply the new style*/
//    lv_obj_t * line1;
//    line1 = lv_line_create(lv_scr_act());
//    lv_line_set_points(line1, pointList, points);     /*Set the points*/
//    lv_obj_add_style(line1, &style_line, 0);
//    lv_obj_center(line1);

        printf("touchSliderInit\n");
    }
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
