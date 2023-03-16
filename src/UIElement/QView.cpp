//
// Created by mac on 2023/3/5.
//
#include <cstdlib>
#include "QView.hpp"
#include "QImage.hpp"
#include "QUIManager.hpp"
#include "lvgl.h"
#include "QIDManager.hpp"

QView *QView::query(QHashID id) {
    return QUIManager::singleton()->query(id);
}

QView *QView::hide() {
    lv_obj_add_flag( lvObj, LV_OBJ_FLAG_HIDDEN );
    return this;
}

QView *QView::show() {
    lv_obj_clear_flag( lvObj, LV_OBJ_FLAG_HIDDEN );
    return this;
}
QView *QView::create( uint16_t w, uint16_t h, int16_t x, int16_t y ) {
    return new QView( {w,h}, {x,y} );
}

QView *QView::none() {
    static auto * none = new QView( NoneType::QViewNoneView );
    return none;
}

QView::QView( QViewNone ) {
    _none = true;
}

QView::QView() {
    id = QIDManager::newVID();
    QUIManager::singleton()->hashView( this, id );
}

QView::QView(QSize size, QPosition pos) {
    lvObj =  lv_obj_create(lv_scr_act());
    _position = pos;
    _size = size;
    this->size( size )->pos( pos );
}

QView::~QView() {

    nodes.clear();

    free(lvObj);
}

QView * QView::insert( QView * view ) {

    if (view->isNone()||isNone()) return this;

    nodes.push_back(view);

    lv_obj_set_parent(view->lvObj,lvObj);

    return this;
}

QView *QView::batchInsert( QView** views ) {

    if (isNone()) return this;

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
    lv_obj_set_style_bg_color(lvObj, {bgColor.b,bgColor.g,bgColor.r},static_cast<lv_style_selector_t>(QViewState::DEFAULT));
    lv_obj_set_style_bg_opa(lvObj, bgColor.a, static_cast<lv_style_selector_t>(QViewState::DEFAULT));
    return this;
}

QView *QView::bg( uint32_t hexRGBA, QViewState forState) {
    return bg( {
       static_cast<uint8_t>((hexRGBA&0xff000000)>>24),
       static_cast<uint8_t>((hexRGBA&0x00ff0000)>>16),
       static_cast<uint8_t>((hexRGBA&0x0000ff00)>>8),
       static_cast<uint8_t>(hexRGBA&0x000000ff)
    }, forState );
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

QView *QView::bg(uint32_t hexRGBA) {
    return bg( hexRGBA, QViewState::any );
}

QView *QView::noScroll() {
    lv_obj_clear_flag( lvObj, LV_OBJ_FLAG_SCROLLABLE);
    return this;
}

QView *QView::radius(uint16_t r) {
    lv_obj_set_style_radius( lvObj, r, 0 );
    return this;
}

QView *QView::checkable() {
    lv_obj_add_flag(lvObj, LV_OBJ_FLAG_CHECKABLE);
    return this;
}

QView *QView::unCheckable() {
    lv_obj_clear_flag(lvObj, LV_OBJ_FLAG_CHECKABLE);
    return this;
}

QView *QView::noBorder() {
    lv_obj_set_style_border_width(lvObj, 0, static_cast<lv_style_selector_t>(QViewState::DEFAULT));
    return this;
}

QView *QView::border( uint16_t weight ) {
    lv_obj_set_style_border_width(lvObj, weight, static_cast<lv_style_selector_t>(QViewState::DEFAULT));
    return this;
}

QView * QView::padding( uint16_t top, uint16_t right, uint16_t bottom, uint16_t left ) {
    paddingTop(top);
    paddingRight(right);
    paddingBottom(bottom);
    paddingLeft(left);
    return this;
}

QView * QView::paddingTop( uint16_t v ) {
    lv_obj_set_style_pad_top(lvObj, v, static_cast<lv_style_selector_t>(QViewState::DEFAULT));
    return this;
}

QView * QView::paddingRight( uint16_t v ) {
    lv_obj_set_style_pad_right(lvObj, v, static_cast<lv_style_selector_t>(QViewState::DEFAULT));

    return this;
}

QView * QView::paddingBottom( uint16_t v ) {
    lv_obj_set_style_pad_bottom(lvObj, v, static_cast<lv_style_selector_t>(QViewState::DEFAULT));

    return this;
}

QView * QView::paddingLeft( uint16_t v ) {
    lv_obj_set_style_pad_left(lvObj, v, static_cast<lv_style_selector_t>(QViewState::DEFAULT));
    return this;
}

QView * QView::noPadding() {
    padding(0,0,0,0);
    return this;
}

QView *QView::checkBg(uint32_t hexColor) {
    return bg( hexColor,QViewState::CHECKED )->checkable();
}

QView *QView::checkBg(QColor bgColor) {
    return bg( bgColor, QViewState::CHECKED )->checkable();
}

QView *QView::bg(QImage *img) {

    _bgImage = img;
    lv_obj_set_style_bg_img_src(lvObj, img->data(), static_cast<lv_style_selector_t>(QViewState::DEFAULT));

    return this;
}

QView *QView::checkBg(QImage *img) {

    _bgImage = img;
    lv_obj_set_style_bg_img_src(lvObj, img->data(), static_cast<lv_style_selector_t>(QViewState::CHECKED));

    return this;
}

QImage *QView::bgImg() {
    return _bgImage;
}

QView *QView::hash(QHashID vid) {
    QUIManager::singleton()->hashView( this, vid );
    return this;
}

QView *QView::create(uint16_t w, uint16_t h) {
    return create(w,h,0,0);
}

QView *QView::bg(QColor bgColor, QColor checkColor) {
    return bg(bgColor)->checkBg(checkColor);
}

QView *QView::bg(uint32_t bgColor, uint32_t checkColor) {
    return bg(bgColor)->checkBg(checkColor);
}

QView *QView::clean() {
    return noScroll()->noPadding()->noBorder()->radius(0);
}

