
include_directories(. ../testHal)

set(LV_MODULE_TOUCHSLIDER_LIBFILES

        demo.c
        )

link_libraries( lvgl png z lv_drivers pthread m )

#add_library(lv_c_module STATIC ${LV_MODULE_TOUCHSLIDER_LIBFILES})

set( LIBRARY_OUTPUT_PATH ${QVGL_OUTPUTDIR}/lib )

add_executable( curveDemo demo.c ../testHal/testHal.c)

#add_executable( pngPreloaderDemo pngPreloader/demo.c )
