//
// Created by sprite on 2023/3/7.
//

#ifndef QUICKVGL_QUIMANAGER_H
#define QUICKVGL_QUIMANAGER_H

#include <list>
#include <unordered_map>

#include "QTypes.hpp"
#include "QImageManager.hpp"
#include "QUILauncher.hpp"
#include "QInput.hpp"
#include "QEvent.hpp"
#include "QDisplay.hpp"
#include "QTimer.hpp"

/**
 * QUIManager is a manager for the application lifecycle management
 */
class QUIManager {

private:

    QUIManager();

    std::unordered_map<QHashID ,QView*> hashViews;

public:

    static QUIManager * singleton(){
        static auto * _instance = new QUIManager();
        return _instance;
    }

    QHashID hashView( QView * view, QHashID id );

    QView * query( QHashID id );

    ~QUIManager();
};


#endif //QUICKVGL_QUIMANAGER_H
