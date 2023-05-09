
include_directories(./ ./3rd/inc)

set(LV_MODULE_FASTCACHE_LIBFILES

        lv_fastCache.h
        lv_fastCache.c

)

add_library(lv_fastCache SHARED ${LV_MODULE_FASTCACHE_LIBFILES})

set( LIBRARY_OUTPUT_PATH ${QVGL_OUTPUTDIR}/lib )

link_libraries( png z )

#add_executable( touchSliderDemo demo.c )

#add_executable( pngPreloaderDemo pngPreloader/demo.c )

