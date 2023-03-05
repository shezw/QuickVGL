
include_directories(../src UIElement UIKit)

set(QVGL_LIBFILES
        src/qvgl.cpp
        src/qvgl.h
        src/UIElement/QView.cpp
        src/UIElement/QView.h
        )

set(SHARED_QVGL STATIC)

add_library(QuickVGL ${SHARED_QVGL}
        qvgl.cpp
        qvgl.h
        UIElement/QView.cpp
        UIElement/QView.h
        )

set( LIBRARY_OUTPUT_PATH ${QVGL_OUTPUTDIR}/lib )
