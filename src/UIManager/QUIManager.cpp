//
// Created by sprite on 2023/3/7.
//
#include "QView.hpp"
#include "QUIManager.hpp"

QUIManager::QUIManager() = default;

QUIManager::~QUIManager() = default;

QView *QUIManager::query( QHashID vid ) {
    assert(hashViews.count(vid) != 0);
    return hashViews[vid];
}

QHashID QUIManager::hashView( QView * view, QHashID vid) {
    hashViews[ vid ] = view;
    return vid;
}
