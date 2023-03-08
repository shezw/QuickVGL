//
// Created by sprite on 2023/3/6.
//

#include "QString.hpp"

QString::QString() = default;

QString::QString( const std::string& txt ) {
    this->set(txt);
}


QString *QString::getByID( QAppIDType id) {

    return new QString( "" );
}
