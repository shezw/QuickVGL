//
// Created by Sprite on 2023/3/16.
//

#include "QImage.hpp"
#include "QImageManager.hpp"
#include "QIDManager.hpp"

QImage::QImage( lv_img_dsc_t *lvImgDsc) {

    _src = QImageManager::singleton()->addResource(lvImgDsc,QIDManager::singleton()->getID());
    initView();
}

const QImageResource *QImage::source() {
    return _src;
}

QImage *QImage::copy() {
    return new QImage( _src );
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

QImage *QImage::create(const std::string &path) {
    return new QImage( path );
}

void QImage::initView() {

    lvObj = lv_img_create(lv_scr_act());

    if ( _src->isFileSource() ) {
        lv_img_set_src(lvObj, _src->getSource());
    }else{
        lv_img_set_src(lvObj, _src->data());
    }

    lv_obj_set_width(lvObj,LV_SIZE_CONTENT);
    lv_obj_set_height(lvObj,LV_SIZE_CONTENT);

    hide();

}

#if LV_USE_SJPG || LV_USE_PNG

QImage::QImage(const std::string& path) {
    _src = QImageManager::singleton()->addResource(path,QIDManager::singleton()->getID());
    initView();
}

QImage::QImage(lv_img_dsc_t *lvImgDsc, const std::string &id) {

}

QImage::QImage(QImageResource *src, const std::string &id) {
    _src = src;
    initView();
}

QImage::QImage(const std::string &path, const std::string &id) {

}

QImage *QImage::create(const std::string &path, const std::string &id) {
    return nullptr;
}
//
//QImage *QImage::size(QSize size) {
//    _src->size(size);
//    return this;
//}

#endif