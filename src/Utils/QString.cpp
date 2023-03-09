//
// Created by sprite on 2023/3/6.
//

#include "QString.hpp"

QString::QString() = default;

QString::QString( const std::string& txt ) {
    set(txt);
    if ( setterCall ) setterCall( this );
}


QString *QString::getByID( QAppIDType id) {

    return new QString( "" );
}

QString &QString::operator=( const std::string& txt) {
    set(txt);
    if ( setterCall ) setterCall( this );
    return *this;
}
