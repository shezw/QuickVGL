//
// Created by Sprite on 2023/3/17.
//

#include "QImageResource.hpp"

QImageResource::QImageResource( lv_img_dsc_t *lvImgDsc) {

    _lvImg = lvImgDsc;
//    _lvObj = lv_img_create(lv_scr_act());
//    lv_img_set_src(_lvObj,_lvImg);

//    lv_obj_set_width(_lvObj,LV_SIZE_CONTENT);
//    lv_obj_set_height(_lvObj,LV_SIZE_CONTENT);

//    lv_obj_add_flag( _lvObj, LV_OBJ_FLAG_HIDDEN );

}

const lv_img_dsc_t *QImageResource::data() {
    return _lvImg;
}

#if LV_USE_SJPG || LV_USE_PNG

QImageResource::QImageResource(const std::string& path) {
    _lvImg   = nullptr;
    _useFile = true;
    _path = path;
//    _lvObj   = lv_img_create(lv_scr_act());
//    lv_img_set_src(_lvObj,_path.c_str());

//    lv_obj_set_width(_lvObj,LV_SIZE_CONTENT);
//    lv_obj_set_height(_lvObj,LV_SIZE_CONTENT);

//    lv_obj_add_flag( _lvObj, LV_OBJ_FLAG_HIDDEN );

}

#endif

std::string QImageResource::getPath() const {
    return _path;
}

const char * QImageResource::getSource() {
    return _path.c_str();
}

bool QImageResource::isFileSource() const {
    return _useFile;
}

//QImageResource *QImageResource::size(QSize size) {
//    lv_obj_set_size( _lvObj, (short)size.w, (short)size.h );
//    return nullptr;
//}
