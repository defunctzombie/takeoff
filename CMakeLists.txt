project (Takeoff)
cmake_minimum_required(VERSION 2.6)

set(target takeoff)

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

#set (QT_USE_QTOPENGL true)
#set (QT_USE_QTXML true)
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )

#needed for moc usage and includes
include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

if (WIN32)
	#http://www.gtk.org/download.html
	#add_definitions("-Wl,-subsystem,window")
	add_definitions(-mwindows)
	set( poppler_base "E:/Roman/poppler-0.10.4")
	include_directories(${poppler_base}/qt4/src)
else ()
	include_directories("/usr/include/poppler/qt4")
endif ()

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
target_link_libraries(${target} ${QT_LIBRARIES} poppler-qt4)
