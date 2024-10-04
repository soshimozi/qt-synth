cmake_minimum_required(VERSION 3.16)

project(synthesizer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PORTAUDIO_BASE_DIR "d:/portaudio")

if(NOT PORTAUDIO_BASE_DIR)
    message(FATAL_ERROR "Please provide the base directory for PortAudio using -DPORTAUDIO_BASE_DIR=<path>")
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    src/audionode.cpp src/audionode.h
    src/audioplayer.cpp src/audioplayer.h
    src/automationnode.cpp src/automationnode.h
    src/blackkey.cpp src/blackkey.h
    src/definitions.h
    src/gainnode.cpp src/gainnode.h
    src/helpers.cpp src/helpers.h
    src/knobcontrol.cpp src/knobcontrol.h
    src/lp12filternode.cpp src/lp12filternode.h
    src/main.cpp
    src/mainwindow.cpp src/mainwindow.h
    src/mixernode.cpp src/mixernode.h
    src/oscillatornode.cpp src/oscillatornode.h
    src/pianokey.cpp src/pianokey.h
    src/spritesheet.cpp src/spritesheet.h
    src/tooltip.cpp src/tooltip.h
)

set(PROJECT_RESOURCES
    images/LittlePhatty.png
    resources/resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(synthesizer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_RESOURCES}
    )

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET qt-test APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(synthesizer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(synthesizer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(synthesizer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

include_directories(${PORTAUDIO_BASE_DIR}/include)
target_link_directories(synthesizer PRIVATE ${PORTAUDIO_BASE_DIR}/build/msvc/x64/ReleaseMinDependency)
target_link_libraries(synthesizer PRIVATE portaudio_x64.lib)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.synthesizer)
endif()

set_target_properties(synthesizer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS synthesizer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(synthesizer)
endif()
