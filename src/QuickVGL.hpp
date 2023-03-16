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

class QuickVGL {

    const static uint32_t DefaultLoopIntervalMS = 20;

public:
    static void init();
    static void init( uint32_t loopInMS );
    static void showVersion();
    static void exit();

};

/**
 * Sugar
 */

#define QV(w,h,x,y) QView::create(w,h,x,y)
#define qview(id) QView::query(id)
#define ImageSet(id,img) QImageManager::singleton()->add(img,id);
#define qimage(id) QImageManager::query(id)

#endif //QUICKVGL_QUICKVGL_HPP
