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
    bool _useFile = false;

    void initView();

public:

    explicit QImage( QViewNone );
    explicit QImage( lv_img_dsc_t * lvImgDsc );
    explicit QImage( QImageResource * src );

    explicit QImage( lv_img_dsc_t * lvImgDsc, const std::string & id  );
    explicit QImage( QImageResource * src, const std::string & id  );

#if LV_USE_SJPG || LV_USE_PNG

    explicit QImage( const std::string& path );
    explicit QImage( const std::string& path, const std::string & id );
    static QImage * create( const std::string& path );
    static QImage * create( const std::string& path, const std::string& id );

#endif

//    QImage * size( QSize size );

    QImage * copy();

    const QImageResource * source();

    static QImage * none();



};


#endif //QUICKVGL_QIMAGE_HPP
