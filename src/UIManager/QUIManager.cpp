//
// Created by sprite on 2023/3/7.
//
#include "QView.hpp"
#include "QUIManager.hpp"

QUIManager::QUIManager() {

}

QUIManager::~QUIManager() {

}

QView *QUIManager::query( QHashID id ) {

    return hashViews[id] ? hashViews[id] : nullptr;
}


QHashID QUIManager::hashView( QView * view, QHashID id) {

    hashViews[ id ] = view;
    return id;
}
