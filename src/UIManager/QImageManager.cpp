//
// Created by Sprite on 2023/3/16.
//

#include "QImageManager.hpp"

QImage *QImageManager::add(QImage *qImg, const std::string& id) {
    this->pool[ id ] = qImg;
    return qImg;
}

QImage *QImageManager::query(const std::string& id) {

    assert( QImageManager::singleton()->pool.count(id) != 0 );

    return QImageManager::singleton()->pool[id];
}

QImage *QImageManager::queryIf(const std::string &id) {

    return QImageManager::singleton()->pool[id] ? QImageManager::singleton()->pool[id] : QImage::none();
}
