//
// Created by Sprite on 2023/3/9.
//

#include "QIDManager.hpp"

QHashID QIDManager::customID( QHashID cid ){

    if ( auto lt = std::find(idSet.begin(), idSet.end(), cid) != idSet.end() )
        idSet.insert( idSet.end(), cid );
    return cid;
}

QHashID QIDManager::customVID( QHashID cid ){

    if ( auto lt = std::find(vidSet.begin(), vidSet.end(), cid) != vidSet.end() )
        vidSet.insert( vidSet.end(), cid );
    return cid;
}

QIDManager::QIDManager() {

}
