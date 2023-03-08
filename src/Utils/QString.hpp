//
// Created by sprite on 2023/3/6.
//

#ifndef QUICKVGL_QSTRING_HPP
#define QUICKVGL_QSTRING_HPP

#include <string>
#include "QValue.hpp"


class QString : public QValue<std::string>{


public:

    QString();
    explicit QString( const std::string& txt );

    static QString * getByID( QAppIDType id );

};


#endif //QUICKVGL_QSTRING_HPP
