//
// Created by sprite on 2023/3/6.
//

#include "QString.hpp"

#include <utility>

QString::QString() = default;

QString::QString( const std::string& txt ) {
    setText(txt);
}

QString *QString::query( QHashID id) {

    return new QString( "" );
}

QString &QString::operator=( const std::string& txt) {
    setText( txt );
    return *this;
}

QString *QString::setText( const std::string& txt ) {
    set(txt);
    for( const auto& caller : setterCalls ){
        caller.second( this );
    }
    return this;
}

QString *QString::combineLambda(QString::SetCall call, QHashID id) {

    setterCalls[id] = std::move(call);

    return this;
}
