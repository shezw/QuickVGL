
include_directories(.)

link_libraries( lvgl png z lv_c_module lv_drivers pthread m )

set( LIBRARY_OUTPUT_PATH ${QVGL_OUTPUTDIR}/lib )

add_executable( touchSliderDemo demo.c ../testHal/testHal.c)
