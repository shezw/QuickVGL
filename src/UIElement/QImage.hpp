//
// Created by Sprite on 2023/3/16.
//

#ifndef QUICKVGL_QIMAGE_HPP
#define QUICKVGL_QIMAGE_HPP

#include <string>
#include "QView.hpp"
#include "QImageResource.hpp"

class QView;
class QImageResource;

class QImage : public QView{

    QImageResource * _src;

    void initView();

public:

    explicit QImage( QViewNone );
    explicit QImage( QImageResource * src );

    const QImageResource * source();

    static QImage * none();

    QImage * copy();

};


#endif //QUICKVGL_QIMAGE_HPP
