set(CROSS_COMPILE 1)
set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_SYSTEM_PROCESSOR ${ARCH})

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)

set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)

set(CMAKE_AR			${TOOLCHAIN_PREFIX}ar)
set(CMAKE_LINKER		${TOOLCHAIN_PREFIX}ld)
set(CMAKE_NM			${TOOLCHAIN_PREFIX}nm)
set(CMAKE_OBJDUMP		${TOOLCHAIN_PREFIX}objdump)
set(CMAKE_RANLIB		${TOOLCHAIN_PREFIX}ranlib)

set(ENV{PKG_CONFIG_PATH} "${QVGL_LOCAL}/lib/pkgconfig")

set(WEBKIT_DEP_PATH     "${QVGL_LOCAL}")

set(CMAKE_FIND_ROOT_PATH ${WEBKIT_DEP_PATH})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)