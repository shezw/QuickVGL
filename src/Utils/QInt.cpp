//
// Created by sprite on 2023/3/6.
//

#include "QInt.hpp"

QInt::QInt() : QValue() {

}

QInt::QInt( int32_t i ) : QValue() {
    this->set( i );
}

QInt *QInt::getByID( QHashID id) {

    return new QInt( 0 );
}
