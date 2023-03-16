//
// Created by Sprite on 2023/3/16.
//

#ifndef QUICKVGL_QIMAGE_HPP
#define QUICKVGL_QIMAGE_HPP

#include "QView.hpp"

class QView;


class QImage : public QView{

    lv_img_dsc_t * _img;

public:

    explicit QImage( QViewNone );
    explicit QImage( lv_img_dsc_t * lvImgDsc );

    QImage * copy();

    const lv_img_dsc_t * data();

    static QImage * none();

};


#endif //QUICKVGL_QIMAGE_HPP
