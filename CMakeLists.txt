cmake_minimum_required(VERSION 3.14)
project(QuickVGL)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE Debug)

set(QVGL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(QVGL_OUTPUTDIR ${QVGL_DIR}/output)

# Export DIRS
set(EXECUTABLE_OUTPUT_PATH ${QVGL_OUTPUTDIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${QVGL_OUTPUTDIR}/lib)


include_directories(${QVGL_DIR} lvgl lvgl/src lv_drivers)

set(QVGL_DIR src)



include_directories(
        ${QVGL_DIR}
        ${QVGL_DIR}/Graphics
        ${QVGL_DIR}/Utils
        ${QVGL_DIR}/UIElement
        ${QVGL_DIR}/UIKit
        ${QVGL_DIR}/UIManager
        ${QVGL_DIR}/Uart
        )

add_subdirectory(lv_drivers)

add_subdirectory(lvgl)

add_subdirectory(src)

add_subdirectory(demo)

