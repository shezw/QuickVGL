#ifndef HV_FBTEST_H

#define HV_FBTEST_H

#define CHIP(CHIPSET) (defined CHIP_##CHIPSET  && CHIP_##CHIPSET)


#if CHIP(SSD202)

#ifndef USE_MEMCPY
    #define USE_MEMCPY 0
#endif

#ifndef USE_DMA
    #define USE_DMA 1
#endif

#endif



#if CHIP(R818)

#ifndef USE_MEMCPY
    #define USE_MEMCPY 0
#endif

#ifndef USE_R818DMA
    #define USE_R818DMA 1
#endif

#endif






#ifndef USE_MEMCPY
#define  USE_MEMCPY 1
#endif

#ifndef USE_DMA
#define USE_DMA 0
#endif

#define OS(OSTYPE) (defined OS_##OSTYPE && OS_##OSTYPE)

#ifdef __APPLE__
#ifndef OS_MAC
    #define OS_MAC
#endif
#endif

#ifdef __linux__
#ifndef OS_LINUX
#define OS_LINUX 1
#endif
#endif

#ifdef _WIN32
#ifndef OS_WIN
#define OS_WIN
#endif
#endif

#endif // HV_FBTEST_H