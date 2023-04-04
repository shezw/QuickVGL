
include_directories(touchSlider)

set(LV_MODULE_LIBFILES

        touchSlider/lv_touchSlider.h
        touchSlider/lv_touchSlider.c

)

link_libraries( lvgl )

add_library(lv_c_module STATIC ${LV_MODULE_LIBFILES})

set( LIBRARY_OUTPUT_PATH ${QVGL_OUTPUTDIR}/lib )



link_libraries( lvgl lv_c_module )

add_executable( touchSliderDemo touchSlider/demo.c )

