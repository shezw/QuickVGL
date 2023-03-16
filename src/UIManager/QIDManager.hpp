//
// Created by Sprite on 2023/3/9.
//

#ifndef QUICKVGL_QIDMANAGER_HPP
#define QUICKVGL_QIDMANAGER_HPP

#include <algorithm>
#include <list>
#include "QTypes.hpp"

class QIDManager {

    QHashID _id = 100000;
    QHashID _vid = 100000;

    /**
     * id   for Values
     */
    std::list<QHashID> idSet;

    /**
     * id   for Views
     */
    std::list<QHashID> vidSet;

    QIDManager();

public:

    static QIDManager * singleton(){
        static auto * _instance = new QIDManager();
        return _instance;
    }

    QHashID getID(){
        while ( isCustomID(_id) ) { }
        _id++;
        if ( _id > 5000000 ) _id = 100000;
        return _id;
    }

    QHashID getVID(){
        while ( isCustomID(_vid) ){}
        _vid++;
        if ( _vid > 5000000 ) _vid = 100000;
        return _vid;
    }

    bool isCustomVID( QHashID cid ){
        auto lt = std::find(vidSet.begin(), vidSet.end(), cid);
        return lt != vidSet.end();
    }

    bool isCustomID( QHashID cid ){
        auto lt = std::find(idSet.begin(), idSet.end(), cid);
        return lt != idSet.end();
    }

    QHashID customID( QHashID cid );

    QHashID customVID( QHashID cid );

    static QHashID newID(){
        return QIDManager::singleton()->getID();
    }

    static QHashID newVID(){
        return QIDManager::singleton()->getVID();
    }

};


#endif //QUICKVGL_QIDMANAGER_HPP
