//
// Created by mac on 2023/3/5.
//
#include <cstdlib>
#include "QView.hpp"
#include "QUIManager.hpp"
#include "lvgl.h"
#include "QIDManager.hpp"

QView *QView::query(QHashID id) {
    return nullptr;
}

QView *QView::hide() {
    lv_obj_add_flag( lvObj, LV_OBJ_FLAG_HIDDEN );
    return this;
}

QView *QView::show() {
    lv_obj_clear_flag( lvObj, LV_OBJ_FLAG_HIDDEN );
    return this;
}

QView::QView() {
    id = QIDManager::newID();
    printf("New QView id(%u)\n",id);
    QUIManager::singleton()->hashView( this, id );
}

QView::QView(QSize size, QPosition pos) {

    _bgColor = {255,255,255};
    _tintColor = {0,0,0};
    _position = pos;
    _size = size;
    lvObj =  lv_obj_create(lv_scr_act());
    this->size( size )->pos( pos )->bg(_bgColor);
}

QView::~QView() {

    nodes.clear();

    free(lvObj);
}

QView * QView::insert( QView * view ) {

    nodes.push_back(view);

    lv_obj_set_parent(view->lvObj,lvObj);

    return this;
}

QView *QView::batchInsert( QView** views ) {
    // TODO
    return this;
}

QSize QView::size() {
    return _size;
}

QPosition QView::pos() {
    return _position;
}

QView *QView::size(QSize size) {
    _size = size;
    lv_obj_set_size( lvObj, (short)size.w, (short)size.h );
    return this;
}

QView *QView::pos(QPosition pos) {
    _position = pos;
    lv_obj_set_pos( lvObj, pos.x, pos.y );
    return this;
}

QView *QView::bg(QColor bgColor) {
    _bgColor = bgColor;
    bg( bgColor, QViewState::any );
    return this;
}

QView *QView::bg(QColor bgColor, QViewState forState) {
    lv_obj_set_style_bg_color(lvObj, {bgColor.b,bgColor.g,bgColor.r}, static_cast<lv_style_selector_t>(forState));
    lv_obj_set_style_bg_opa(lvObj, bgColor.a, static_cast<lv_style_selector_t>(forState));
    return this;
}

QColor QView::bg() {

    return bg( QViewState::DEFAULT );
}

QColor QView::bg( QViewState forState ) {

    return _bgColor;
}

QColor QView::tintColor() {
    return _tintColor;
}

QView *QView::tintColor(QColor color) {
    _tintColor = color;
    return this;
}
