//
// Created by mac on 2023/3/5.
//

#include "QView.hpp"
#include "lvgl.h"

QView *QView::getByID(QAppIDType id) {
    return nullptr;
}

QView *QView::hide() {
    return this;
}

QView *QView::show() {
    return this;
}

QView::QView() {

    lvObj =  lv_obj_create(nullptr);
}

QView::QView(QSize size, QPosition pos) {

    lvObj =  lv_obj_create(nullptr);
    this->size( size )->pos( pos );
}

QView::~QView() {

    nodes.clear();

    free(lvObj);
}

QView * QView::insert( QView& view) {

    nodes.push_back(view);

    lv_obj_set_parent(view.lvObj,lvObj);

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
    bg( bgColor, QViewState::DEFAULT );
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
