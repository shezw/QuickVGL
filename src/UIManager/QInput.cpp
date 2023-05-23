//
// Created by Sprite on 2023/3/8.
//

#include "QInput.hpp"
#include <SDL2/SDL.h>
#include "sdl/sdl.h"

QInput::QInput() {

#if OS_LINUX
    indev = nullptr;
    indev_drv = (_lv_indev_drv_t*) malloc(sizeof (_lv_indev_drv_t));
    deviceName = DEFAULT_EVDEV_NAME;
#endif
}

QInput *QInput::init() {
    return QInput::init( DEFAULT_EVDEV_NAME );
}

QInput *QInput::init(const std::string &devName) {

    auto * inputDevice = new QInput();

    inputDevice->deviceName = devName;
    return inputDevice->initDevice();
}

QInput* QInput::initDevice() {

#ifndef WIN32
    setenv("DBUS_FATAL_WARNINGS", "0", 1);
#endif

#if OS_LINUX
#if USE_EVDEV
    evdev_init( (char *) deviceName.c_str() );
    lv_indev_drv_init(indev_drv);             /*Descriptor of a input device driver*/
    indev_drv->read_cb = evdev_read;
#endif

#if USE_MOUSE | USE_EVDEV | USE_LINMICE | USE_LIBINPUT
    indev_drv->type = LV_INDEV_TYPE_POINTER;
#endif

    indev = lv_indev_drv_register(indev_drv);

    lv_obj_t *cursor_img = lv_img_create(lv_scr_act());
    lv_img_set_src(cursor_img, LV_SYMBOL_UP);
    lv_indev_set_cursor(indev, cursor_img);

#elif OS_MAC || WIN32

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

#endif

    return this;
}
