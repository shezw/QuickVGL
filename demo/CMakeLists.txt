
link_libraries( QuickVGL lv_drivers lv_c_module pthread m)

include_directories(../lvgl ../lvgl/src assets ../lv_c_module ../lv_c_module/touchSlider)

add_executable( qdemo main.cpp main.hpp )