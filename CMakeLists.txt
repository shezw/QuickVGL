cmake_minimum_required(VERSION 3.14)
project(QuickVGL)

set(CMAKE_CXX_STANDARD 14)

if(WIN32)
    set(OS_TYPE win)
    add_compile_definitions(OS_WIN=1)
endif(WIN32)

if(APPLE)
    set(OS_TYPE mac)
    add_compile_definitions(OS_MAC=1)
endif(APPLE)

if(UNIX AND NOT APPLE)
    set(OS_TYPE linux)
    add_compile_definitions(OS_LINUX=1)
endif()

if (${CHIP_SSD202})

add_compile_definitions(CHIP_SSD202=1)

set(SSD202_SOURCE_PATH /home/shezw/ssd202-sdk)


## SSD202 SDK
set(SSD202_PROJECT ${SSD202_SOURCE_PATH}/project)
#
include_directories(
        ${SSD202_PROJECT}/release/include
)
#
link_directories(
        ${SSD202_PROJECT}/release/nvr/i2m/common/glibc/8.2.1/mi_libs/dynamic
        ${SSD202_PROJECT}/release/nvr/i2m/common/glibc/8.2.1/ex_libs/dynamic
)

link_libraries(
        mi_common mi_sys pthread dl
)


endif ()

message("Your os type is: ${OS_TYPE}")

set(QVGL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(QVGL_OUTPUTDIR ${QVGL_LOCAL})

# Export DIRS
set(EXECUTABLE_OUTPUT_PATH ${QVGL_OUTPUTDIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${QVGL_OUTPUTDIR}/lib)

set(ENV{PKG_CONFIG_PATH} "${QVGL_LOCAL}/lib/pkgconfig")

set(CMAKE_FIND_ROOT_PATH ${QVGL_LOCAL})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(LV_DIR lvgl-8.3.6)
if (USE_LVGL9)
    set(LV_DIR lvgl)
endif ()

if (USE_TSLIB)

    add_compile_definitions(USE_TSLIB=true)

    include_directories(${QVGL_LOCAL}/include)
    link_directories(${QVGL_LOCAL}/lib)
    link_libraries(ts dl)

endif ()

if (USE_LIB_PNG)

    include_directories(${QVGL_DIR}/local/include)
    include_directories(3rd/libpng-1.6.39)
    add_subdirectory(3rd/libpng-1.6.39)
    link_libraries(png)
    link_libraries(z)

endif ()

if (USE_FREETYPE)
    find_package(Freetype REQUIRED)
    include_directories(${QVGL_LOCAL}/include)
    include_directories(${FREETYPE_INCLUDE_DIRS})
    link_directories(${QVGL_LOCAL}/lib)
    link_libraries(freetype z)
endif ()

include_directories(${QVGL_DIR} ${LV_DIR} ${LV_DIR}/src lv_drivers configs)

if (USE_SDL)
    find_package(SDL2 REQUIRED)
    include_directories(${QVGL_LOCAL}/include)
    include_directories(${SDL2_INCLUDE_DIRS})
    link_directories(${QVGL_LOCAL}/lib)
    link_libraries(${SDL2_LIBS})
endif ()

set(QVGL_SRC src)

include_directories(
        ${QVGL_SRC}
        ${QVGL_SRC}/Graphics
        ${QVGL_SRC}/Utils
        ${QVGL_SRC}/UIElement
        ${QVGL_SRC}/UIKit
        ${QVGL_SRC}/UIWidget
        ${QVGL_SRC}/UIManager
)

if(APPLE OR WIN32)

    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
    link_directories(${QVGL_LOCAL}/lib)
    link_libraries(SDL2)
endif ()

add_subdirectory(lv_drivers)

add_subdirectory(${LV_DIR})

add_subdirectory(lv_c_module)

add_subdirectory(src)

if (APPLE)
#    add_subdirectory(macSimulator)
endif ()

add_subdirectory(demo)

#add_subdirectory(c_app)
