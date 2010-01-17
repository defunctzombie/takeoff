project (Takeoff)
cmake_minimum_required(VERSION 2.6)

set(target takeoff)

set(ENV{PKG_CONFIG_PATH} $ENV{PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig)

find_package(PkgConfig REQUIRED)
pkg_search_module(POPPLER REQUIRED poppler-qt4)
include_directories(${POPPLER_INCLUDE_DIRS})
set(LIBS ${LIBS} ${POPPLER_LIBRARIES})

set (SRCS
    main.cpp
    MainWindow.cpp
    Viewer.cpp
    Shape.cpp
    Page.cpp
    ColorSwatch.cpp
    
    shapes/Area.cpp
    shapes/Line.cpp
)

set (QT_USE_QTOPENGL true)
#set (QT_USE_QTXML true)
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
set(LIBS ${LIBS} ${QT_LIBRARIES})

#needed for moc usage and includes
include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

qt4_wrap_cpp ( MOC_SRCS
    Viewer.hpp
    MainWindow.hpp
    ColorSwatch.hpp
)

qt4_wrap_ui (UI_FILES
    ui/MainWindow.ui
)

qt4_add_resources (QRC_FILES
    ui/icons/icons.qrc
)

add_executable(${target} ${SRCS} ${MOC_SRCS} ${QRC_FILES} ${UI_FILES})
target_link_libraries(${target} ${LIBS})
