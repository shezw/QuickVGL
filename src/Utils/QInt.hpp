//
// Created by sprite on 2023/3/6.
//

#ifndef QUICKVGL_QINT_HPP
#define QUICKVGL_QINT_HPP

#include "QValue.hpp"

class QInt: public QValue<int32_t>{


public:

    QInt();
    explicit QInt( int int32_t );


    static QInt * getByID( QHashID id );

};


#endif //QUICKVGL_QINT_HPP
