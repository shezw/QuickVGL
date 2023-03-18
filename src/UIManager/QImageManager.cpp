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
    return pool[id] = new QImage( new QImageResource(path) );
}

QImage *QImageManager::add(const std::string& path, QHashID id) {
    return idPool[id] = new QImage( new QImageResource(path));
}

QImage *QImageManager::add(QImageResource *qImgSrc, const std::string &id) {
//    printf("Create image by QImageRes %p  id(%s)\n", qImgSrc, id.c_str());
    return pool[id] = new QImage(qImgSrc);
}

QImage *QImageManager::add(QImageResource *qImgSrc, QHashID id) {
    return idPool[id] = new QImage(qImgSrc);
}

bool QImageManager::hasSource(const std::string &path) {
    return sourcePool.count(path)>0;
}

QImageResource *QImageManager::getSource(const std::string &path) {
    assert(hasSource(path)>0);
    return sourcePool[path];
}

bool QImageManager::hasSourceByPath(const std::string &path) {
    return fileCache.count(path)>0;
}

QImageResource *QImageManager::getSourceByPath(const std::string &path) {
    assert(hasSource(path)>0);
    return fileCache[path];
}

QImage *QImageManager::query(const std::string& id) {

    auto * manager = QImageManager::singleton();

    // if QImage already exist
    if ( manager->pool.count(id) )
        return manager->pool[id];

    // id as File path but no QImage
    if ( manager->hasSourceByPath(id) )
        return manager->add( manager->getSourceByPath(id), id );

    // id as hash id  but no QImage
    if ( manager->hasSource(id) )
        return manager->add( manager->getSource(id), id );

    assert( manager->pool.count(id) != 0 );
    return nullptr;
}

QImage *QImageManager::query( QHashID id) {

    auto * manager = QImageManager::singleton();

    // if QImage already exist
    if ( manager->idPool.count(id) )
        return manager->idPool[id];

    assert( manager->idPool.count(id) != 0 );
    return nullptr;
}

QImage *QImageManager::queryIf(const std::string &id) {

    return QImageManager::singleton()->pool[id] ? QImageManager::singleton()->pool[id] : QImage::none();
}

QImageResource *QImageManager::addResource(QImageResource *qImgSrc, const std::string &id) {
    if ( sourcePool.count(id) > 0 && qImgSrc != sourcePool[id] )
        delete sourcePool[id];
    if (qImgSrc->isFileSource()) {
        if (fileCache.count(qImgSrc->getPath())>0) return fileCache[qImgSrc->getPath()];
        fileCache[qImgSrc->getPath()] = qImgSrc;
    }
    return sourcePool[ id ] = qImgSrc;
}

QImageResource *QImageManager::addResource(QImageResource *qImgSrc, QHashID id) {
    if ( idSourcePool.count(id) > 0 && qImgSrc != idSourcePool[id] )
        delete idSourcePool[id];
    if (qImgSrc->isFileSource()) {
        if (fileCache.count(qImgSrc->getPath())>0) return fileCache[qImgSrc->getPath()];
        fileCache[qImgSrc->getPath()] = qImgSrc;
    }
    return idSourcePool[ id ] = qImgSrc;
}

QImageResource *QImageManager::addResource(const std::string &path, const std::string &id) {
    if ( fileCache.count(path) > 0 && fileCache[path] )
        return sourcePool[id] = fileCache[path];
    return sourcePool[id] = fileCache[path] = new QImageResource(path);
}

QImageResource *QImageManager::addResource(const std::string &path, QHashID id) {
    if ( fileCache.count(path) > 0 && fileCache[path] )
        return idSourcePool[id] = fileCache[path];
    return idSourcePool[id] = fileCache[ path ] = new QImageResource(path);
}

QImageResource *QImageManager::querySource(const std::string &id) {
    if ( QImageManager::singleton()->sourcePool.count(id) > 0 ) // Image File ID
        return QImageManager::singleton()->sourcePool[id];
    if ( QImageManager::singleton()->fileCache[id] )            // FilePath
        return QImageManager::singleton()->fileCache[id];
    assert( QImageManager::singleton()->sourcePool.count(id) != 0 );
    return nullptr;
}

QImageResource *QImageManager::querySource(QHashID id) {
    assert( QImageManager::singleton()->idSourcePool.count(id) != 0 );
    return QImageManager::singleton()->idSourcePool[id];
}
