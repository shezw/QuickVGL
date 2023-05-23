//
// Created by Sprite on 2023/3/8.
//

#ifndef QUICKVGL_QINPUT_HPP
#define QUICKVGL_QINPUT_HPP

#include <string>
#include <lv_drv_conf.h>

#define DEFAULT_EVDEV_NAME "/dev/input/event0"

#if OS_LINUX
#if USE_EVDEV
#include "indev/evdev.h"
#endif

#include <linux/input.h>

#elif OS_MAC
#include "sdl/sdl.h"
#include "indev/mouse.h"
#include "indev/keyboard.h"
#include "indev/mousewheel.h"

#endif

class QInput {

private:

    lv_indev_drv_t * indev_drv = nullptr;
    lv_indev_t * indev = nullptr;

    std::string deviceName;

public:

    QInput();

    QInput * initDevice();

    static QInput * init();
    static QInput * init( const std::string &devName  );

};


#endif //QUICKVGL_QINPUT_HPP
