//
// Created by 志伟佘 on 2023/5/14.
//

#include "testHal.h"
#include "lvgl.h"

#ifndef SYSTEM_RESPONSE_TIME
#define SYSTEM_RESPONSE_TIME 5  /* Default to 5 milliseconds to keep the system responsive */
#endif

#if 1

/*
    Linux frame buffer like /dev/fb0
*/

/*******************
*       DEFINE
********************/

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
*       GLOBAL
********************/
static fbdev_struct fbdev_info;  /* framebuffer */
struct fb_fix_screeninfo finfo;

static screen_struct screen_info;    /* scree */
static indev_struct indev_info;  /* touchpad data */
#if (INPUT_READ_MODE==3)
int flags;
#endif


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

void testHalInit(){


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


}

void testHalLoop(){

    pthread_create(&pt, NULL, tick_thread, NULL);

    while (true){

        lv_task_handler();
        usleep(SYSTEM_RESPONSE_TIME * 1000);

//        lv_tick_inc(SYSTEM_RESPONSE_TIME);
//        sleep(1);
    }

}

#elif OS_MAC

#include <SDL2/SDL.h>
#include "sdl/sdl.h"

typedef struct _lv_disp_t lv_disp_t;

void testHalInit(){

    sdl_init( SDL_HOR_RES, SDL_VER_RES );
    void * lvContextBuffer = malloc( SDL_HOR_RES * SDL_VER_RES * 4 );

    lv_disp_t * lvDisplay = NULL;
    lv_disp_drv_t * lvDisplayDriver;
    lv_disp_draw_buf_t * lvDispDrawBuf;


    lvDisplayDriver = (lv_disp_drv_t *) malloc( sizeof(lv_disp_drv_t));
    bzero(lvDisplayDriver,sizeof(lv_disp_drv_t));

    lv_disp_drv_init( lvDisplayDriver );

    lvDispDrawBuf = (lv_disp_draw_buf_t *) malloc( sizeof(lv_disp_draw_buf_t));
    bzero(lvDispDrawBuf,sizeof(lv_disp_draw_buf_t));

    lvDisplayDriver->flush_cb = sdl_display_flush;
    lv_disp_draw_buf_init( lvDispDrawBuf, lvContextBuffer, NULL, SDL_HOR_RES * SDL_VER_RES);


    if (!lvDisplay) lvDisplay = (struct _lv_disp_t *) malloc( sizeof(struct _lv_disp_t));
    bzero(lvDisplay,sizeof(struct _lv_disp_t));

    /* Initialize with basic configuration*/
//    if (!lvDisplayDriver) {

        /*Create a display*/
        lvDisplayDriver->draw_buf = lvDispDrawBuf;
        lvDisplayDriver->antialiasing = 1;
        lvDisplayDriver->sw_rotate = 1;
        lvDisplayDriver->hor_res = (int16_t) SDL_HOR_RES;
        lvDisplayDriver->ver_res = (int16_t) SDL_VER_RES;
//        lvDisplayDriver->full_refresh = 1;
        lvDisplayDriver->rotated = LV_DISP_ROT_NONE;
//        lvDisplayDriver->rotated = LV_DISP_ROT_90;
//        lvDisplayDriver->user_data = this;
        lvDisplayDriver->draw_ctx_size = 1024*1024*4;
//    } else
//        memcpy(lvDisplayDriver, drv, sizeof(lv_disp_drv_t));

    lvDisplay = lv_disp_drv_register( lvDisplayDriver );

    lv_theme_t * th = lv_theme_default_init(lvDisplay, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
    lv_disp_set_theme(lvDisplay, th);

    lv_group_t * g = lv_group_create();
    lv_group_set_default(g);

    static lv_indev_drv_t indev_drv_mouse;
    lv_indev_drv_init(&indev_drv_mouse); /*Basic initialization*/
    indev_drv_mouse.type = LV_INDEV_TYPE_POINTER;

    /*This function will be called periodically (by the library) to get the mouse position and state*/
    indev_drv_mouse.read_cb = sdl_mouse_read;
    lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_mouse);

    static lv_indev_drv_t indev_drv_mousewheel;
    lv_indev_drv_init(&indev_drv_mousewheel); /*Basic initialization*/
    indev_drv_mousewheel.type = LV_INDEV_TYPE_ENCODER;
    indev_drv_mousewheel.read_cb = sdl_mousewheel_read;

    lv_indev_t * enc_indev = lv_indev_drv_register(&indev_drv_mousewheel);
    lv_indev_set_group(enc_indev, g);


}

void loopHandler(uint32_t ms) {

    lv_timer_handler();

#ifdef SDL_APPLE
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
#if USE_MOUSE != 0
        mouse_handler(&event);
#endif

#if USE_KEYBOARD
        keyboard_handler(&event);
#endif

#if USE_MOUSEWHEEL != 0
        mousewheel_handler(&event);
#endif
    }
#endif
}

static bool _exit = false;
static int loopInMS = 5;

/**
 * A task to measure the elapsed time for LittlevGL
 * @param data unused
 * @return never return
 */
static int tick_thread(void * data)
{
    (void)data;

    while(true) {
#if USE_SDL
        SDL_Delay(5);   /*Sleep for 5 millisecond*/
#endif
        lv_tick_inc(5); /*Tell LittelvGL that 5 milliseconds were elapsed*/
    }

    return 0;
}

void testHalLoop(){

    SDL_CreateThread(tick_thread, "tick", NULL);


    while ( !_exit ) {

        SDL_Delay(loopInMS);
        loopHandler( loopInMS );
    }
}

#endif