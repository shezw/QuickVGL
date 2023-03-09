//
// Created by Sprite on 2023/3/8.
//

#ifndef QUICKVGL_QUILAUNCHER_HPP
#define QUICKVGL_QUILAUNCHER_HPP

#include <stdint.h>


class QUILauncher {

private:

    bool _exit = false;

    QUILauncher();

public:

    static QUILauncher * singleton() {
        static auto _uiLauncher = new QUILauncher();
        return _uiLauncher;
    }

    void run( uint32_t loopInMS ) const;
    void kill();

    static void handler(uint32_t ms);

};


#endif //QUICKVGL_QUILAUNCHER_HPP
