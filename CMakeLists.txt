cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

project (Takeoff VERSION 1.0)
set(target takeoff)

set (SRCS
    main.cpp
    MainWindow.cpp
    Viewer.cpp
    Page.cpp
    ColorSwatch.cpp
)

set (SHAPE_SRCS
    Shape.cpp
    shapes/Area.cpp
    shapes/Line.cpp
    shapes/Count.cpp
    shapes/Check.cpp
)

find_package (Qt5 REQUIRED COMPONENTS Widgets Core Gui Xml)

# needed for moc usage and includes
include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

set(RESOURCE_FILES ui/icons/icons.qrc)

qt5_wrap_cpp (MOC_SRCS
    Viewer.hpp
    MainWindow.hpp
    ColorSwatch.hpp
)

qt5_wrap_ui (UI_FILES
    ui/MainWindow.ui
)

qt5_add_resources (QRC_FILES
    ui/icons/icons.qrc
)

find_path(poppler-include NAMES poppler/qt5/poppler-qt5.h)
find_library(poppler-libs NAMES poppler-qt5)

include_directories(${poppler-include})

set(CPACK_BUNDLE_NAME takeoff)
set(CPACK_GENERATOR Bundle)
SET(MACOSX_BUNDLE_ICON_FILE app.icns) 

SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/app.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/project.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

add_executable(${target} MACOSX_BUNDLE ${SRCS} ${MOC_SRCS} ${QRC_FILES} ${UI_FILES}
	${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/app.icns
	${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/project.icns
)

add_library(shape STATIC ${SHAPE_SRCS})

target_link_libraries(shape Qt5::Widgets Qt5::Gui Qt5::Core Qt5::Xml)
target_link_libraries(${target} shape Qt5::Widgets Qt5::Gui Qt5::Core ${poppler-libs})