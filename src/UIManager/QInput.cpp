//
// Created by Sprite on 2023/3/8.
//

#include "QInput.hpp"
#include <linux/input.h>

#define DEFAULT_EVDEV_NAME "/dev/input/event0"

QInput::QInput() {

    indev = nullptr;
    indev_drv = (_lv_indev_drv_t*) malloc(sizeof (_lv_indev_drv_t));
    deviceName = DEFAULT_EVDEV_NAME;
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

    return this;
}
