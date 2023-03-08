//
// Created by sprite on 2023/3/6.
//

#ifndef QUICKVGL_QLABEL_HPP
#define QUICKVGL_QLABEL_HPP

#include "QView.hpp"
#include "QString.hpp"
#include <string>

class QLabel : QView {


private:

    QString _text;

public:

    explicit QLabel( QString t );
    explicit QLabel( const std::string& t ) {
        _text = QString(t);
    }

    QLabel text( QString t );
    QLabel append( QString t );
};


#endif //QUICKVGL_QLABEL_HPP
