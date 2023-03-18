//
// Created by Sprite on 2023/3/17.
//

#ifndef QUICKVGL_QIMAGERESOURCE_HPP
#define QUICKVGL_QIMAGERESOURCE_HPP

/**
 * Used for store image resource that for QImageView
 */

#include <string>
#include "QView.hpp"
#include "lvgl.h"

class QImageResource{

    bool _useFile = false;

    lv_img_dsc_t * _lvImg;
    std::string _path;

public:

    explicit QImageResource( lv_img_dsc_t * lvImgDsc );

#if LV_USE_SJPG || LV_USE_PNG

    explicit QImageResource( const std::string& path );

#endif

    bool isFileSource() const;

    const lv_img_dsc_t * data();

    const char * getSource();

    std::string getPath() const;

};


#endif //QUICKVGL_QIMAGERESOURCE_HPP
