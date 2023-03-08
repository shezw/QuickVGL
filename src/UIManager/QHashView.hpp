//
// Created by sprite on 2023/3/7.
//

#ifndef QUICKVGL_QHASHVIEW_HPP
#define QUICKVGL_QHASHVIEW_HPP

#include "QTypes.hpp"
#include "QUIElement.hpp"

class QHashView {

public:
    QHashView(QView * view, QAppIDType i);

    QView * view;
    QAppIDType id;
};


#endif //QUICKVGL_QHASHVIEW_HPP
