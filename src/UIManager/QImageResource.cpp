//
// Created by Sprite on 2023/3/17.
//

#include "QImageResource.hpp"
#include "QIDManager.hpp"

void QImageResource::initId() {
    id = QIDManager::newID();
}

QImageResource::QImageResource() {
    initId();
}

QImageResource::QImageResource( lv_img_dsc_t *lvImgDsc) {
    initId();
    _useFile = false;
    _lvImg = lvImgDsc;
}

const lv_img_dsc_t *QImageResource::data() {
    return _lvImg;
}

#if LV_USE_SJPG || LV_USE_PNG

QImageResource::QImageResource(const std::string& path) {
    initId();
    _lvImg   = nullptr;
    _useFile = true;
    _path = path;
}

QImageResource::QImageResource(QString *qPath) {
    initId();
    _lvImg   = nullptr;
    _useFile = true;
    _path    = qPath->value();
    qPath->combineLambda([this]( QString * txt ){
        this->_path = txt->value();
    }, this->id );
}

#endif

std::string QImageResource::getPath() const {
    return _path;
}

const char * QImageResource::getSource() {
    return _path.c_str();
}

bool QImageResource::isFileSource() const {
    return _useFile;
}
