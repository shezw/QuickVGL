//
// Created by sprite on 2023/3/6.
//

#ifndef QUICKVGL_QSTRING_HPP
#define QUICKVGL_QSTRING_HPP

#include <string>
#include <functional>
#include <unordered_map>
#include "QValue.hpp"

class QLabel;

class QString : public QValue<std::string>{


public:

    QString();

    explicit QString( const std::string& txt );

    static QString * query( QHashID id );

    QString * setText( const std::string & txt );

    QString & operator = ( const std::string & txt );

    typedef std::function<void(QString * txt)>SetCall;

    QString * combineLambda( SetCall call, QHashID id );
    std::unordered_map<QHashID ,SetCall> setterCalls;
};


#endif //QUICKVGL_QSTRING_HPP
