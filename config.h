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







#ifndef USE_MEMCPY
#define  USE_MEMCPY 1
#endif

#ifndef USE_DMA
#define USE_DMA 0
#endif






#endif // HV_FBTEST_H