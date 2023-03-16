//
// Created by Sprite on 2023/3/16.
//

#include "QImage.hpp"

QImage::QImage( lv_img_dsc_t *lvImgDsc) {

    _img = lvImgDsc;

    lvObj = lv_img_create(lv_scr_act());
    lv_img_set_src(lvObj,_img);

    lv_obj_set_width(lvObj,LV_SIZE_CONTENT);
    lv_obj_set_height(lvObj,LV_SIZE_CONTENT);

    pos(0,0);
    hide();
}

const lv_img_dsc_t *QImage::data() {
    return _img;
}

QImage *QImage::copy() {
    return new QImage( _img );
}

QImage::QImage(QViewNone){
    _img = nullptr;
    _none = true;
}

QImage *QImage::none() {
    auto * noneImg = new QImage(NoneType::QViewNoneImage);
    return noneImg;
}
