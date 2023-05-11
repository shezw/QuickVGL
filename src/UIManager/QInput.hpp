//
// Created by Sprite on 2023/3/8.
//

#ifndef QUICKVGL_QINPUT_HPP
#define QUICKVGL_QINPUT_HPP

#include <string>
#include <lv_drv_conf.h>

#if USE_EVDEV
#include "indev/evdev.h"
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
