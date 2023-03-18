//
// Created by Sprite on 2023/3/16.
//

#ifndef QUICKVGL_QIMAGE_HPP
#define QUICKVGL_QIMAGE_HPP

#include <string>
#include "QView.hpp"
#include "QImageResource.hpp"

class QView;
class QImageResource;

class QImage : public QView{

    QImageResource * _src;

    void initView();
    void initSrc();

public:

    explicit QImage( QViewNone );
    explicit QImage( QImageResource * src );

    const QImageResource * source();

    static QImage * none();

    QImage * copy();


    QImage * setSource( QImageResource * source );

    typedef std::function<void(QString * txt)>SetSourceCall;

    QImage * combineLambda( SetSourceCall call, QHashID id );
    std::unordered_map<QHashID ,SetSourceCall> setterPathCalls;



};


#endif //QUICKVGL_QIMAGE_HPP
