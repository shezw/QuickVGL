//
// Created by Sprite on 2023/3/16.
//

#include "QImage.hpp"
#include "QImageManager.hpp"
#include "QIDManager.hpp"

const QImageResource *QImage::source() {
    return _src;
}

QImage::QImage(QImageResource *src) {
    _src = src;
    initView();
}

QImage::QImage(QViewNone){
    _src = nullptr;
    _none = true;
}

QImage *QImage::none() {
    auto * noneImg = new QImage(NoneType::QViewNoneImage);
    return noneImg;
}

void QImage::initSrc() {

    if ( _src->isFileSource() ) {
        lv_img_set_src(lvObj, _src->getSource());
    }else{
        lv_img_set_src(lvObj, _src->data());
    }

}

void QImage::initView() {

    lvObj = lv_img_create(lv_scr_act());

    initSrc();

    lv_obj_set_width(lvObj,LV_SIZE_CONTENT);
    lv_obj_set_height(lvObj,LV_SIZE_CONTENT);

    hide();

}

QImage *QImage::copy() {
    return new QImage( _src );
}

QImage *QImage::setSource(QImageResource *source) {

    _src = source;

    initSrc();

    return this;
}
