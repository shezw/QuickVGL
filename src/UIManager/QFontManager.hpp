//
// Created by Sprite on 2023/3/17.
//

#ifndef QUICKVGL_QFONTMANAGER_HPP
#define QUICKVGL_QFONTMANAGER_HPP

#include <unordered_map>
#include <string>
#include "QTypes.hpp"
#include "QFont.hpp"

class QFontManager {

private:

    std::unordered_map<std::string, QFont *> pool;
    std::unordered_map<QHashID, QFont *> idPool;

    QFontManager() = default;

public:

    static QFontManager *singleton() {
        static auto _fontManager = new QFontManager();
        return _fontManager;
    }

    QFont * add( QFont * qFont, const std::string& id );

    QFont * add( QFont * qFont, QHashID id );

    QFont * add( const std::string& path, const std::string& id );
    QFont * add( const std::string& path, QHashID id );

    static QFont * query( const std::string& id );
    static QFont * query(QHashID id);

};

#endif //QUICKVGL_QFONTMANAGER_HPP
