//
// Created by Sprite on 2023/3/16.
//

#include "QImageManager.hpp"
#include "QIDManager.hpp"

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
    return pool[id] = new QImage(path);
}

QImage *QImageManager::add(const std::string& path, QHashID id) {
    return idPool[id] = new QImage(path);
}

QImage *QImageManager::add(QImageResource *qImgSrc, const std::string &id) {
//    printf("Create image by QImageRes %p  id(%s)\n", qImgSrc, id.c_str());
    return pool[id] = new QImage(qImgSrc);
}

QImage *QImageManager::add(QImageResource *qImgSrc, QHashID id) {
    return idPool[id] = new QImage(qImgSrc);
}

bool QImageManager::hasSource(const std::string &path) {
    return sourceCache.count(path)>0;
}

QImageResource *QImageManager::getSource(const std::string &path) {
    assert(hasSource(path)>0);
    return sourceCache[path];
}

QImage *QImageManager::query(const std::string& id) {

    auto * manager = QImageManager::singleton();
    if ( manager->hasSource(id) ) return manager->add( manager->getSource(id), id );

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

QImageResource *QImageManager::addResource(QImageResource *qImgSrc, const std::string &id) {
    if ( sourcePool.count(id) > 0 || qImgSrc != sourcePool[id] ) delete sourcePool[id];
    return sourcePool[ id ] = sourceCache[id] = qImgSrc;
}

QImageResource *QImageManager::addResource(QImageResource *qImgSrc, QHashID id) {
    if ( idSourcePool.count(id) > 0 || qImgSrc != idSourcePool[id] ) delete idSourcePool[id];
    return idSourcePool[ id ] = sourceCache[ QHashIDString(id) ] = qImgSrc;
}

QImageResource *QImageManager::addResource(const std::string &path, const std::string &id) {
    if ( sourceCache.count(path) > 0 )
        return sourcePool[id] = sourceCache[path];
    return sourcePool[id] = sourceCache[id] = new QImageResource(path);
}

QImageResource *QImageManager::addResource(const std::string &path, QHashID id) {
    if ( sourceCache.count(path) > 0 )
        return idSourcePool[id] = sourceCache[path];
    return idSourcePool[id] = sourceCache[ QHashIDString(id) ] = new QImageResource(path);
}

QImageResource *QImageManager::querySource(const std::string &id) {
    assert( QImageManager::singleton()->sourcePool.count(id) != 0 );
    return QImageManager::singleton()->sourcePool[id];
}

QImageResource *QImageManager::querySource(QHashID id) {
    assert( QImageManager::singleton()->idSourcePool.count(id) != 0 );
    return QImageManager::singleton()->idSourcePool[id];
}
