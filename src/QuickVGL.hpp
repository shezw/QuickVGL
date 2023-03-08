//
// Created by mac on 2023/3/5.
//

#ifndef QUICKVGL_QUICKVGL_HPP
#define QUICKVGL_QUICKVGL_HPP

#define QVGL_VERSION "0.1.0"

#include "QGraphics.hpp"

#include "QUtils.hpp"

#include "QUIElement.hpp"

#include "QUIKit.hpp"

#include "QUIManager.hpp"
#include "QUILauncher.hpp"
#include "QInput.hpp"
#include "QEvent.hpp"
#include "QDisplay.hpp"

class QuickVGL {

    const static uint32_t DefaultLoopIntervalMS = 20;

public:
    static void init();
    static void init( uint32_t loopInMS );
    static void showVersion();

};

#endif //QUICKVGL_QUICKVGL_HPP
