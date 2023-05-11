cmake_minimum_required(VERSION 3.14)
project(QuickVGL)

set(CMAKE_CXX_STANDARD 14)

if(WIN32)
    set(OS_TYPE win)
endif(WIN32)

if(APPLE)
    set(OS_TYPE mac)
endif(APPLE)

if(UNIX AND NOT APPLE)
    set(OS_TYPE linux)
endif()

message("Your os type is: ${OS_TYPE}")

set(QVGL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(QVGL_OUTPUTDIR ${QVGL_DIR}/output)

# Export DIRS
set(EXECUTABLE_OUTPUT_PATH ${QVGL_OUTPUTDIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${QVGL_OUTPUTDIR}/lib)

set(LV_DIR lvgl-8.3.6)
if (USE_LVGL9)
    set(LV_DIR lvgl)
endif ()

if (USE_LIB_PNG)

    include_directories(${QVGL_DIR}/local/include)
    include_directories(3rd/libpng-1.6.39)
    add_subdirectory(3rd/libpng-1.6.39)
    link_libraries(png)
    link_libraries(z)

endif ()

find_package(Freetype 2.4.2 REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
link_directories(${QVGL_LOCAL}/lib)
link_libraries(freetype z)

include_directories(${QVGL_DIR} ${LV_DIR} ${LV_DIR}/src lv_drivers)

if (USE_SDL)
    include_directories( build/SDL2-2.26.5/include build/SDL2-2.26.5/include-config-release )
endif ()

set(QVGL_SRC src)

include_directories(
        ${QVGL_SRC}
        ${QVGL_SRC}/Graphics
        ${QVGL_SRC}/Utils
        ${QVGL_SRC}/UIElement
        ${QVGL_SRC}/UIKit
        ${QVGL_SRC}/UIManager
)

if (USE_SDL)
    add_subdirectory(3rd/SDL2-2.26.5)
    link_libraries(3rd/SDL2)
endif ()

add_subdirectory(lv_drivers)

add_subdirectory(${LV_DIR})

add_subdirectory(lv_c_module)

add_subdirectory(src)

if (APPLE)
    add_subdirectory(macSimulator)
endif ()

add_subdirectory(demo)

add_subdirectory(c_app)
