//
// Created by sprite on 2023/3/6.
//

#ifndef QUICKVGL_QLABEL_HPP
#define QUICKVGL_QLABEL_HPP

#include "QView.hpp"
#include "QString.hpp"
#include "QFont.hpp"
#include <string>

class QLabel : public QView {

private:

    QString * _text{};

public:

    explicit QLabel( QString * t );
    explicit QLabel( int i );
    explicit QLabel(const std::string &t): QLabel( new QString(t) ){}
    QLabel * bind( QString * t );

    QLabel * text( QString * t );
    QLabel * text( QInt * t );
    QLabel * append( QString * t );

    QLabel * font( QFont * ft );

    QLabel * textColor( QColor color );
    QLabel * textColor( uint32_t hexColor );

    static QLabel * query( QHashID id );
};


#endif //QUICKVGL_QLABEL_HPP
