#!/bin/bash

######################################
##          Project Info

QVGL_DIR=$(cd $(dirname $0);cd ./; pwd)
QVGL_SRC_DIR=${QVGL_DIR}/src

platform=""
toolchain="/cross_compiler/ssd202/bin/arm-linux-gnueabihf-"
debug="Debug"
skip=0
help=0
clean=0
arch="x86"

MAKE_CORES=64

while getopts ":kct:d:p:" opt
do
    case $opt in
        p)
          platform=$OPTARG
          echo "  Platform:         ${platform}"
        ;;
        t)
          toolchain=$OPTARG
          echo "  Toolchain:         ${toolchain}"
        ;;
        d)
          debug=$OPTARG
          echo "  Build debug:      ${debug}"
        ;;
        k)
          skip=1
          echo "  Skip Confirm:     ${skip}"
        ;;
        c)
          clean=1
          echo "  clean build:     ${clean}"
        ;;
        h)
          help=1
        ;;
        ?)
          echo "  Undefined arg: $OPTARG"
        ;;
    esac
done

if [[
    ${toolchain} == "" ||
    ${debug} == "" || (${debug} != 'Release' && ${debug} != 'Debug' && ${debug} != 'RelWithDebInfo' && ${debug} != 'MinSizeRel') ||
    ${platform} == "" || (${platform} != 'x86' && ${platform} != 'ssd202' && ${platform} != 'r818' && ${platform} != 'raspi' && ${platform} != 'mac')
]]; then
  help=1
fi

    # CMAKE_BUILD_TYPE   Release  Debug  RelWithDebInfo  MinSizeRel

if [[ ${help} == 1 ]]; then
    echo "
    -p [ platform ]  default: x86
    * Specific a Chip Platform ( x86, ssd202, r818, raspi, mac )

    -t [ toolchain ]  default: /cross_compiler/ssd202/bin/arm-linux-gnueabihf-
    * Specific a Toolchain Path  (Only available on cross compile)

    -d [ debug ]    default: Release
    Build debug type  ( Release, Debug, RelWithDebInfo, MinSizeRel )

    -k [ skip ]
    Skip enter confirm

    -c [ clean ]
    Clean old files

    -t [ testing ]
    Show testing configure only
    "
    exit
fi


function SolidMessage() {

    suc=$1
    msg=$2
    err=$3

    if [ ${suc} -ne 0 ]; then
        echo -e "\e[31m\e[1m   ${err}\e[0m\n"
        exit
    else
        echo -e "\e[32m\e[1m   ${msg}\e[0m\n"
    fi
}

function ConfirmSuspend(){

  msg=$1
  if [[ ${skip} -eq 0 ]]; then
  echo "  ${1}  exit with ^C"
  read var
  var=""
  fi
}

function GotoDir() {
    _dir=$1
    mkdir -p "${_dir}"
    cd ${_dir}
}

function ResetDir() {
    _dir=$1
    echo "   Rebuild Dir"
    cd ${QVGL_DIR}
    rm -rf "${_dir}"
    GotoDir ${_dir}
}

CHIP_SET=""

if [[ ${platform} == 'ssd202' ]]; then
    arch='arm'
    CMAKE_CROSS_FILE=${QVGL_DIR}/cross.cmake
    MESON_CROSS_FILE=${QVGL_DIR}/cross.meson.txt
    CHIP_SET="-DCHIP_SSD202=YES"

      CROSS_COMPILER="arm-linux-gnueabihf"
      CROSS_COMPILER_HOST="arm-linux-gnueabihf"
      CROSS_COMPILER_DIR="/cross_compiler/${platform}"

    CROSS_COMPILER_HOST="arm-linux-gnueabihf"
fi

if [[ ${platform} == 'x86' ]]; then
    arch='amd64'
    CMAKE_CROSS_FILE=
fi

if [[ ${platform} == 'mac' ]]; then
    arch='arm64'
    CMAKE_CROSS_FILE=
fi


if [[ ${debug} == 'Release' ]]; then
  export CFLAGS="-O2"
  export CXXFLAGS="-O2"
elif [[ ${debug} == 'RelWithDebugInfo' ]]; then
  export CFLAGS="-g -O2"
  export CXXFLAGS="-g -O2"
elif [[ ${debug} == 'MinSizeRel' ]]; then
  export CFLAGS="-O3"
  export CXXFLAGS="-O3"
elif [[ ${debug} == 'Debug' ]]; then
  export CFLAGS="-g"
  export CXXFLAGS="-g"
fi


MAKE_WITH_MULTI_CORES="make -j${MAKE_CORES}"
MAKE_MAKE_INSTALL="make install -j${MAKE_CORES}"
LOCAL_DIR="${PWD}/local/${platform}"
BUILD_PREFIX="--prefix=${LOCAL_DIR}"
TARGET_PREFIX="--host=${CROSS_COMPILER_HOST}"

echo -e "
  ----------------------------------------------------------------------

  \e[33m\e[1mProject Info\e[0m

  ARCH:                  ${arch}
  Platform:              ${platform}
  Clean:                 ${clean}

  Project Dir:           ${QVGL_DIR}
  QuickVGL Dir:          ${QVGL_SRC_DIR}
  CMAKE_CROSS_FILE       ${CMAKE_CROSS_FILE}

  CROSS_COMPILER         ${CROSS_COMPILER}

  Release(NDebug) mode:  ${debug}

  MAKE_WITH_MULTI_CORES  ${MAKE_WITH_MULTI_CORES}
  MAKE_MAKE_INSTALL      ${MAKE_MAKE_INSTALL}
  BUILD_PREFIX           ${BUILD_PREFIX}
  TARGET_PREFIX          ${TARGET_PREFIX}
  BUILD_ONLY_DEPS        ${BUILD_ONLY_DEPS}
  REBUILD                ${rebuild}

  LDFLAGS                ${LDFLAGS}
  CPPFLAGS               ${CPPFLAGS}
  CFLAGS                 ${CFLAGS}
  CXXFLAGS               ${CXXFLAGS}
  "


ConfirmSuspend "
Press enter to continue configure..."

if [ ${clean} == 1 ]; then
    rm -rf build
fi

SDL_ARGS=" -DSDL_WAYLAND=OFF -DSDL_X11=OFF -DSDL_VULKAN=OFF -DSDL_AUDIO=OFF -DSDL_VIDEO=OFF -DSDL_OPENGLES=OFF -DSDL_OPENGL=OFF -DSDL_IBUS=OFF"
USE_LVGL9=" -DUSE_LVGL9=ON"
USE_WS_SERVER=" -DUSE_WS_SERVER=ON"
#USE_TSLIB="-DUSE_TSLIB=ON"
USE_FREETYPE="-DUSE_FREETYPE=ON"
USE_LIB_PNG="-DUSE_LIB_PNG=ON"
#USE_FREETYPE=""
#USE_LIB_PNG=""
#USE_LVGL9=""
USE_TSLIB=""
USE_SDL="OFF"

if [ ${platform} == 'mac' ]; then
    USE_TSLIB=""
    USE_SDL="-DUSE_SDL=ON"
fi

export LDFLAGS="-L${LOCAL_DIR}/lib"
export CPPFLAGS="-I${LOCAL_DIR}/include"
export PKG_CONFIG_LIBDIR="${LOCAL_DIR}/lib/pkgconfig:${LOCAL_DIR}/lib/share/pkgconfig"

#export PKG_CONFIG_LIBDIR="${LOCAL_DIR}/lib/pkgconfig:${LOCAL_DIR}/share/pkgconfig"

ConfirmSuspend "
Press enter to continue make zlib..."

GotoDir ${QVGL_DIR}/build/${platform}/zlib

cmake ../../../3rd/zlib-1.2.13 -DQVGL_LOCAL=${LOCAL_DIR} -DCMAKE_INSTALL_PREFIX=${LOCAL_DIR} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CROSS_FILE} -DCMAKE_BUILD_TYPE=${debug} -DTOOLCHAIN_PREFIX=${toolchain} -DARCH=${arch}

${MAKE_WITH_MULTI_CORES}
${MAKE_MAKE_INSTALL}

if [[ ${USE_TSLIB} != "" ]]; then

    ConfirmSuspend "
    Press enter to continue make tslib..."

    GotoDir ${QVGL_DIR}/build/${platform}/tslib

    cmake ../../../3rd/tslib-1.22 -DQVGL_LOCAL=${LOCAL_DIR} -DCMAKE_INSTALL_PREFIX=${LOCAL_DIR} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CROSS_FILE} -DCMAKE_BUILD_TYPE=${debug} -DTOOLCHAIN_PREFIX=${toolchain} -DARCH=${arch}

    ${MAKE_WITH_MULTI_CORES}
    ${MAKE_MAKE_INSTALL}

fi


ConfirmSuspend "
Press enter to continue make pixman..."

GotoDir ${QVGL_DIR}/build/${platform}/pixman-0.42.2
export PKG_CONFIG_PATH=${LOCAL_DIR}
meson build ../../../3rd/pixman-0.42.2 --cross-file=${MESON_CROSS_FILE} --buildtype=release --default-library=static -Dlibpng=disabled -Dtls=disabled -Dopenmp=disabled

cd build
ninja
ninja install


ConfirmSuspend "
Press enter to continue make cairo..."

GotoDir ${QVGL_DIR}/build/${platform}/cairo-1.16.0
export PKG_CONFIG_PATH=${LOCAL_DIR}
../../../3rd/cairo-1.16.0/configure --enable-xlib=no --disable-dependency-tracking ${BUILD_PREFIX} ${TARGET_PREFIX} --disable-directfb --disable-win32 --enable-gl=no --enable-ft=no --enable-interpreter=no --enable-pdf=no --enable-png=no --enable-ps=no --enable-svg=no --enable-qt=no --enable-drm=no

${MAKE_WITH_MULTI_CORES}
${MAKE_MAKE_INSTALL}


#AddLibDep "cairo-1.16.0" "${SHARED_LIB_ON} --enable-directfb --disable-win32 --enable-gl=no --enable-ft=yes --enable-interpreter=no --enable-pdf=yes --enable-png=yes --enable-ps=yes --enable-svg=yes --enable-qt=no --enable-drm=no"



ConfirmSuspend "
Press enter to continue make png..."

GotoDir ${QVGL_DIR}/build/${platform}/libpng-1.6.39

cmake ../../../3rd/libpng-1.6.39 -DPNG_STATIC=YES -DQVGL_LOCAL=${LOCAL_DIR} -DCMAKE_INSTALL_PREFIX=${LOCAL_DIR} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CROSS_FILE} -DCMAKE_BUILD_TYPE=${debug} -DTOOLCHAIN_PREFIX=${toolchain} -DARCH=${arch}

${MAKE_WITH_MULTI_CORES}
${MAKE_MAKE_INSTALL}


ConfirmSuspend "
Press enter to continue make freetype..."

GotoDir ${QVGL_DIR}/build/${platform}/freetype

cmake ../../../3rd/freetype -DQVGL_LOCAL=${LOCAL_DIR} -DCMAKE_INSTALL_PREFIX=${LOCAL_DIR} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CROSS_FILE} -DCMAKE_BUILD_TYPE=${debug} -DTOOLCHAIN_PREFIX=${toolchain} -DARCH=${arch}

${MAKE_WITH_MULTI_CORES}
${MAKE_MAKE_INSTALL}


if [ ${platform} == "mac" ]; then

ConfirmSuspend "
Press enter to continue make SDL2..."

GotoDir ${QVGL_DIR}/build/${platform}/sdl

cmake ../../../3rd/SDL-2.26.5 -DQVGL_LOCAL=${LOCAL_DIR} -DCMAKE_INSTALL_PREFIX=${LOCAL_DIR} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CROSS_FILE} -DCMAKE_BUILD_TYPE=${debug} -DTOOLCHAIN_PREFIX=${toolchain} -DARCH=${arch}

${MAKE_WITH_MULTI_CORES}
${MAKE_MAKE_INSTALL}

fi

ConfirmSuspend "
Press enter to continue make QuickVGL..."

GotoDir ${QVGL_DIR}/build/${platform}

cmake ../../ -DQVGL_LOCAL=${LOCAL_DIR} -DCMAKE_INSTALL_PREFIX=${LOCAL_DIR} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CROSS_FILE} -DCMAKE_BUILD_TYPE=${debug} -DTOOLCHAIN_PREFIX=${toolchain} -DARCH=${arch} ${CHIP_SET} ${USE_FREETYPE} ${USE_LVGL9} ${USE_LIB_PNG} ${USE_TSLIB}

ConfirmSuspend "
Press enter to continue make..."

${MAKE_WITH_MULTI_CORES}

