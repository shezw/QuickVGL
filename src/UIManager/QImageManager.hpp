//
// Created by Sprite on 2023/3/16.
//

#ifndef QUICKVGL_QIMAGEMANAGER_HPP
#define QUICKVGL_QIMAGEMANAGER_HPP

#include <unordered_map>
#include <string>
#include "QTypes.hpp"
#include "QImage.hpp"
#include "QImageResource.hpp"

class QImageManager {

private:

    std::unordered_map<std::string ,QImage*> pool;
    std::unordered_map<QHashID , QImage*> idPool;

    std::unordered_map<std::string ,QImageResource*> sourcePool;
    std::unordered_map<QHashID , QImageResource*> idSourcePool;

    std::unordered_map<std::string ,QImageResource*> fileCache;

    QImageManager() = default;

public:

    static QImageManager * singleton() {
        static auto _imageManager = new QImageManager();
        return _imageManager;
    }

    QImage * add( QImage * qImg, const std::string& id );
    QImage * add( QImageResource * qImgSrc, const std::string& id );
    QImage * add( lv_img_dsc_t * lvImgDsc, const std::string& id ) { return add( new QImage( new QImageResource(lvImgDsc) ), id ); }

    QImage * add( QImage * qImg, QHashID id );
    QImage * add( QImageResource * qImgSrc, QHashID id );
    QImage * add( lv_img_dsc_t * lvImgDsc, QHashID id ) { return add( new QImage( new QImageResource(lvImgDsc) ), id ); }

    QImage * add( const std::string& path, const std::string& id );
    QImage * add( const std::string& path, QHashID id );

    bool hasSource(const std::string &path);
    QImageResource * getSource( const std::string& path );

    bool hasSourceByPath(const std::string &path);
    QImageResource * getSourceByPath(const std::string &path);

    QImageResource * addResource( QImageResource * qImgSrc, const std::string& id );
    QImageResource * addResource( lv_img_dsc_t * lvImgDsc, const std::string& id ) { return addResource( new QImageResource( lvImgDsc ), id ); }

    QImageResource * addResource( QImageResource * qImgSrc, QHashID id );
    QImageResource * addResource( lv_img_dsc_t * lvImgDsc, QHashID id ) { return addResource( new QImageResource( lvImgDsc ), id ); }

    QImageResource * addResource( const std::string& path, const std::string& id );
    QImageResource * addResource( const std::string& path, QHashID id );

    static QImage * query( const std::string& id );
    static QImage * query( QHashID id );

    static QImageResource * querySource( const std::string& id );
    static QImageResource * querySource(QHashID id);

    static QImage * queryIf( const std::string& id );

};


#endif //QUICKVGL_QIMAGEMANAGER_HPP
