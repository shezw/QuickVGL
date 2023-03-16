
link_libraries( QuickVGL )

include_directories(../lvgl ../lvgl/src assets)

add_executable( qdemo main.cpp main.hpp assets/testimg.c )