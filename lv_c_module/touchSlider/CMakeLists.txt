
include_directories(.)

set(LV_MODULE_TOUCHSLIDER_LIBFILES

        lv_touchSlider.h
        lv_touchSlider.c

        bigicon.c
        )

link_libraries( lvgl lv_drivers lv_c_module pthread m )

add_library(lv_c_module STATIC ${LV_MODULE_TOUCHSLIDER_LIBFILES})

set( LIBRARY_OUTPUT_PATH ${QVGL_OUTPUTDIR}/lib )

add_executable( touchSliderDemo demo.c )

#add_executable( pngPreloaderDemo pngPreloader/demo.c )
