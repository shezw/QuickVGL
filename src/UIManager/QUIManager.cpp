//
// Created by sprite on 2023/3/7.
//
#include "QView.hpp"
#include "QUIManager.hpp"

UIManager::UIManager() {

}

UIManager::~UIManager() {

}

QView *UIManager::getByID(QAppIDType id) {

    QHashView * hview = nullptr;

    std::for_each(this->hashViews.begin(), this->hashViews.end(),
                  [&id, &hview]( QHashView v ) { if ( id == v.id ) hview = &v; }
                  );

    if( !hview ){

        this->hashView( *(hview = new QHashView( new QView(), id )) );
    }
    return hview->view;
}



QAppIDType UIManager::hashView( QView * view, QAppIDType id) {

    this->hashViews.emplace_back( view, id );

    return id;
}

QAppIDType UIManager::hashView(QHashView hView) {

    this->hashViews.push_back( hView );
    return hView.id;
}

