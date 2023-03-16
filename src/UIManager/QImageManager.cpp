//
// Created by Sprite on 2023/3/16.
//

#include "QImageManager.hpp"

QImage *QImageManager::add(QImage *qImg, const std::string& id) {
    /** Release old resource */
    if ( pool.count(id) > 0 || qImg != pool[id] ) delete pool[id];
    return pool[ id ] = qImg;
}

QImage *QImageManager::add(QImage *qImg, QHashID id) {
    /** Release old resource */
    if ( idPool.count(id) > 0 || qImg != idPool[id] ) delete idPool[id];
    return idPool[ id ] = qImg;
}

QImage *QImageManager::add(const std::string& path, const std::string &id) {
    auto * img = new QImage(path);
    pool[id] = img;
    return img;
}

QImage *QImageManager::add(const std::string& path, QHashID id) {
    auto * img = new QImage(path);
    idPool[id] = img;
    return img;
}

QImage *QImageManager::query(const std::string& id) {
    assert( QImageManager::singleton()->pool.count(id) != 0 );
    return QImageManager::singleton()->pool[id];
}

QImage *QImageManager::query( QHashID id) {
    assert( QImageManager::singleton()->idPool.count(id) != 0 );
    return QImageManager::singleton()->idPool[id];
}

QImage *QImageManager::queryIf(const std::string &id) {

    return QImageManager::singleton()->pool[id] ? QImageManager::singleton()->pool[id] : QImage::none();
}
