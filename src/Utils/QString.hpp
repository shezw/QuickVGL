//
// Created by sprite on 2023/3/6.
//

#ifndef QUICKVGL_QSTRING_HPP
#define QUICKVGL_QSTRING_HPP

#include <string>
#include <functional>
#include "QValue.hpp"

class QLabel;

class QString : public QValue<std::string>{


public:

    QString();
    explicit QString( const std::string& txt );

    static QString * getByID( QAppIDType id );

    QString & operator = ( const std::string & txt );

    typedef std::function<void(QString * txt)>SetCall;

    SetCall setterCall = nullptr;
};


#endif //QUICKVGL_QSTRING_HPP
