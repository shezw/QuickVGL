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

#include "QUIWidget.hpp"

#include "QUIManager.hpp"

class QuickVGL {

    const static uint32_t DefaultLoopIntervalMS = 20;

public:
    static void init();
    static void init( int window_w, int window_h );
    static void init( uint32_t loopInMS );
    static void showVersion();
    static void exit();

};

/**
 * Sugar
 */
/*  Resource Set  */
//  Add Image Resource in to ImageManager
#define ImageSet(id,img)    QImageManager::singleton()->addResource(img,id);
//  Add Font to FontManager
#define FontSet(id,font)    QFontManager::singleton()->add(font,id);

/* QElements */
#define QV(w,h,x,y)         QView::create(w,h,x,y)
#define QI(path)            (new QImage(new QImageResource(path)))
#define QL(text)            (new QLabel(text))

/* QElement Query */
#define qview(id)           QView::query(id)
#define qlabel(id)          QLabel::query(id)
#define qimage(id)          QImageManager::query(id)
#define qimgres(id)         QImageManager::querySource(id)
#define qfont(id)           QFontManager::query(id)

#endif //QUICKVGL_QUICKVGL_HPP
