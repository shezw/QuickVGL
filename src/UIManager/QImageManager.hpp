//
// Created by Sprite on 2023/3/16.
//

#ifndef QUICKVGL_QIMAGEMANAGER_HPP
#define QUICKVGL_QIMAGEMANAGER_HPP

#include <list>
#include <unordered_map>
#include <string>
#include "QTypes.hpp"
#include "QImage.hpp"

class QImageManager {

private:

    std::unordered_map<std::string ,QImage*> pool;

    QImageManager() = default;

public:

    static QImageManager * singleton() {
        static auto _uiLauncher = new QImageManager();
        return _uiLauncher;
    }

    QImage * add( QImage * qImg, const std::string& id );
    QImage * add( lv_img_dsc_t * lvImgDsc, const std::string& id ) { return add( new QImage( lvImgDsc ), id ); }

    QImage * add( const std::string& path, const std::string& id );

    static QImage * query( const std::string& id );
    static QImage * queryIf( const std::string& id );
};


#endif //QUICKVGL_QIMAGEMANAGER_HPP
