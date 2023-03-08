//
// Created by mac on 2023/3/5.
//

#include "QuickVGL.hpp"
#include <iostream>

void QuickVGL::showVersion() {
    std::cout << "QuickVGL Version: " << QVGL_VERSION << std::endl;
}

void QuickVGL::init( const uint32_t loopInMS ) {
    lv_init();
}

void QuickVGL::init() {
    QuickVGL::init( QuickVGL::DefaultLoopIntervalMS );
}