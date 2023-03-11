
include_directories(../src Graphics Utils UIElement UIKit UIManager Uart)

set(QVGL_LIBFILES

        Graphics/QGraphics.hpp
        Graphics/FrameBuffer.hpp
        Graphics/FrameBuffer.cpp

        Utils/QTypes.hpp
        Utils/QUtils.hpp

        Utils/QValue.hpp
        Utils/QString.hpp
        Utils/QString.cpp
        Utils/QInt.hpp
        Utils/QInt.cpp

        Utils/QFont.hpp
        Utils/QFont.cpp

        UIElement/QUIElement.hpp

        UIElement/QView.hpp
        UIElement/QView.cpp
        UIElement/QLabel.hpp
        UIElement/QLabel.cpp

        UIKit/QUIKit.hpp
        UIKit/QPage.hpp
        UIKit/QPage.cpp
        UIKit/QNav.hpp
        UIKit/QNav.cpp
        UIKit/QTab.hpp
        UIKit/QTab.cpp

        UIManager/QEvent.hpp
        UIManager/QEvent.cpp

        UIManager/QDisplay.hpp
        UIManager/QDisplay.cpp

        UIManager/QInput.hpp
        UIManager/QInput.cpp

        UIManager/QTimer.hpp
        UIManager/QTimer.cpp

        UIManager/QIDManager.hpp
        UIManager/QIDManager.cpp

        UIManager/QUIManager.hpp
        UIManager/QUIManager.cpp

        UIManager/QUILauncher.hpp
        UIManager/QUILauncher.cpp


        Uart/QRingbuffer.h
        Uart/QRingbuffer.c        
        Uart/QUart.h      
        Uart/QUart.c           

        QuickVGL.hpp
        QuickVGL.cpp

        )

link_libraries( lvgl pthread)

set(SHARED_QVGL STATIC)

add_library(QuickVGL ${SHARED_QVGL} ${QVGL_LIBFILES})

set( LIBRARY_OUTPUT_PATH ${QVGL_OUTPUTDIR}/lib )
