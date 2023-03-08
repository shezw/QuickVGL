//
// Created by sprite on 2023/3/7.
//

#ifndef QUICKVGL_QUIMANAGER_H
#define QUICKVGL_QUIMANAGER_H

#include <list>
#include "QTypes.hpp"
#include "QHashView.hpp"

/**
 * UIManager is a manager for the application lifecycle management
 */
class UIManager {

private:

    UIManager();

    std::vector<QView*> uiViews;
    std::list<QHashView> hashViews;

public:

    static UIManager * singleton(){
        static auto * _instance = new UIManager();
        return _instance;
    }

    QAppIDType hashView( QView * view, QAppIDType id );

    QAppIDType hashView( QHashView hView );

    QView * getByID( QAppIDType id );

    ~UIManager();
};


#endif //QUICKVGL_QUIMANAGER_H
