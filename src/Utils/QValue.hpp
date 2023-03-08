//
// Created by sprite on 2023/3/6.
//

#ifndef QUICKVGL_QVALUE_HPP
#define QUICKVGL_QVALUE_HPP

#include "QTypes.hpp"

template <class T>
class QValue{

private:

    T _v;

public:

    QValue<T> * set( T v ) {
        this->_v = v;
        return this;
    }

    T value() { return this->_v; }

//    template<class vType>
//    QValue bind( void * (call), vType v );
//
//    QValue unBindAll();
//    QValue unBind();

};

#endif //QUICKVGL_QVALUE_HPP
