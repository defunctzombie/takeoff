project (Takeoff)
cmake_minimum_required(VERSION 2.6)

set(target takeoff)

if (MINGW)
	add_definitions(-Wl,-subsystem,windows)
endif()

set (SRCS
    main.cpp
    MainWindow.cpp
    Viewer.cpp
    Shape.cpp
    Page.cpp
    ColorSwatch.cpp
    
    shapes/Area.cpp
    shapes/Line.cpp
    shapes/Count.cpp
)

set (QT_USE_QTOPENGL true)
set (QT_USE_QTXML true)

find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )

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

set(CPACK_GENERATOR Bundle)
SET(MACOSX_BUNDLE_ICON_FILE app.icns) 

SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/app.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/project.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

add_executable(${target} MACOSX_BUNDLE WIN32 ${SRCS} ${MOC_SRCS} ${QRC_FILES} ${UI_FILES}
${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/app.icns
${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/project.icns
)
target_link_libraries(${target} ${QT_LIBRARIES} poppler-qt4)

if (APPLE)
	install(TARGETS takeoff BUNDLE DESTINATION . COMPONENT Runtime RUNTIME DESTINATION bin COMPONENT Runtime)

	SET(plugin_dest_dir ${target}.app/Contents/MacOS)
	SET(qtconf_dest_dir ${target}.app/Contents/Resources)

	INSTALL(CODE "
	    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
	    " COMPONENT Runtime)

		set(APPS ${CMAKE_INSTALL_PREFIX}/${target}.app) 

	set(DIRS ${QT_LIBRARY_DIRS} /usr/local/lib) 

	INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)

	if (${QT_MAC_USE_COCOA} STREQUAL "1")
	   install(DIRECTORY ${QT_LIBRARY_DIR}/QtGui.framework/Resources/qt_menu.nib DESTINATION ${APPS}/Contents/Resources COMPONENT Runtime)
	endif()

	include(InstallRequiredSystemLibraries)

	INSTALL(CODE "
		file(GLOB_RECURSE QTPLUGINS
	      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
	   include(BundleUtilities)
	   fixup_bundle(\"${APPS}\"   \"${QTPLUGINS}\"   \"${DIRS}\")
	   " COMPONENT Runtime)

endif()

set(CPACK_BINARY_DRAGNDROP ON)
INCLUDE(CPack)

