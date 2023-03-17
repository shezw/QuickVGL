//
// Created by Sprite on 2023/3/17.
//

#include "QFontManager.hpp"

QFont *QFontManager::add(QFont *qImg, const std::string& id) {
    /** Release old resource */
    if ( pool.count(id) > 0 || qImg != pool[id] ) delete pool[id];
    return pool[ id ] = qImg;
}

QFont *QFontManager::add(QFont *qImg, QHashID id) {
    /** Release old resource */
    if ( idPool.count(id) > 0 || qImg != idPool[id] ) delete idPool[id];
    return idPool[ id ] = qImg;
}

QFont *QFontManager::add(const std::string& path, const std::string &id) {
    auto * img = new QFont(path);
    pool[id] = img;
    return img;
}

QFont *QFontManager::add(const std::string& path, QHashID id) {
    auto * img = new QFont(path);
    idPool[id] = img;
    return img;
}

QFont *QFontManager::query(const std::string& id) {
    assert( QFontManager::singleton()->pool.count(id) != 0 );
    return QFontManager::singleton()->pool[id];
}

QFont *QFontManager::query( QHashID id) {
    assert( QFontManager::singleton()->idPool.count(id) != 0 );
    return QFontManager::singleton()->idPool[id];
}
