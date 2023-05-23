include_directories(./touchSlider ./testHal)

set(LV_MODULE_TOUCHSLIDER_LIBFILES

        ./touchSlider/lv_touchSlider.h
        ./touchSlider/lv_touchSlider.c

    )

add_library(lv_c_module STATIC ${LV_MODULE_TOUCHSLIDER_LIBFILES})

add_subdirectory(touchSlider)
#add_subdirectory(fastCache)
#add_subdirectory(cairoDemo)
add_subdirectory(curveDemo)
