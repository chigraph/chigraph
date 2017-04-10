# Fix for CXX11 feature detection of pre-3.1 CMake
if(${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR} LESS 3.1)
        include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/CheckCXX11Features.cmake)
endif()

set(KDE_SKIP_TEST_SETTINGS ON)

# Check that the dependencies dirs exist
if ("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
	if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/kf5-debug")
		message(WARNING "Run setup.py first; trying to find system KF5")
	endif()
else()
	if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/kf5-release")
		message(WARNING "Run setup.py first; trying to find system KF5")
	endif()
endif()

# make sure they have pulled the submodules
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/nodeeditor/CMakeLists.txt")
	message(FATAL_ERROR "Pull the submodules first, run `git submodule update --init`")
endif()

# use the downloaded KF5 libraries
if ("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
	set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/kf5-debug)
else()
	set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/kf5-release)
endif()

find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_AUTORCC ON)

include(KDEInstallDirs)
include(KDECMakeSettings)
#include(KDECompilerSettings)
include(FeatureSummary)

# find qt
find_package(Qt5 5.6.0 REQUIRED COMPONENTS Gui Script Xml Core Widgets Network PrintSupport)
find_package(Qt5 5.6.0 COMPONENTS DBus)

find_package(KF5 REQUIRED COMPONENTS Archive TextEditor JobWidgets DBusAddons KIO Crash Sonnet SyntaxHighlighting Parts GuiAddons ItemViews Config ConfigWidgets Auth Codecs Completion GlobalAccel Service WindowSystem CoreAddons I18n XmlGui WidgetsAddons TextWidgets IconThemes)

# make sure QtMain is linked
cmake_policy(SET CMP0020 NEW)

if (CG_BUILD_DEBUGGER)
	add_subdirectory(debugger)
endif()

set(GUI_SRCS
	main.cpp
	mainwindow.cpp
	chigraphnodemodel.cpp
	functionview.cpp
	functiondetails.cpp
	paramlistwidget.cpp
	subprocessoutputview.cpp
	modulebrowser.cpp
	execparamlistwidget.cpp
	thememanager.cpp
	localvariables.cpp
	functioninouts.cpp
	typeselector.cpp
	functiontabview.cpp
	launchconfigurationmanager.cpp
	launchconfigurationdialog.cpp
)

set(GUI_RESOURCES
	chigraphgui.qrc
)

set(GUI_HEADERS
	mainwindow.hpp
	chigraphnodemodel.hpp
	functionview.hpp
	subprocessoutputview.hpp
	paramlistwidget.hpp
	functiondetails.hpp
	modulebrowser.hpp
	execparamlistwidget.hpp
	thememanager.hpp
	localvariables.hpp
	functioninouts.hpp
	typeselector.hpp
	functiontabview.hpp
	launchconfigurationmanager.hpp
	launchconfigurationdialog.hpp
)

add_executable(chigraphgui ${GUI_SRCS} ${GUI_HEADERS} ${GUI_RESOURCES})

# for some reason cmake doesn't pick up on me wanting to do this...idk
set_target_properties(chigraphgui PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


target_link_libraries(chigraphgui
	Qt5::Widgets 
	KF5::Completion 
	KF5::TextEditor 
	KF5::Crash 
	KF5::Service 
	KF5::CoreAddons 
	KF5::I18n 
	KF5::Auth 
	KF5::XmlGui 
	KF5::WidgetsAddons 
	KF5::TextWidgets 
	KF5::IconThemes 
	nodes 
	chigraph
)

if (CG_BUILD_DEBUGGER)
	target_link_libraries(chigraphgui chigraphguidebugger)
endif()

if(${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR} LESS 3.1)
	CXX11_CHECK_FEATURE("variadic_templates" 2555 HAS_CXX11_VARIADIC_TEMPLATES)
else(${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR} GREATER 3.1 OR ${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR} EQUAL 3.1)
	target_compile_features(chigraphgui PRIVATE cxx_variable_templates)
endif()

if(UNIX)
	target_compile_options(chigraphgui PRIVATE "-fexceptions")
endif()

# copy dlls
if(WIN32)
	add_custom_target(copydlls ALL)
	foreach(QTLIB
		Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Script
		Qt5::Network Qt5::DBus Qt5::PrintSupport Qt5::Xml
		KF5::Crash KF5::Parts KF5::KIOCore KF5::KIOWidgets
		KF5::JobWidgets KF5::TextEditor KF5::SyntaxHighlighting
		KF5::DBusAddons KF5::Archive KF5::ConfigWidgets
		KF5::GuiAddons KF5::ItemViews KF5::Codecs KF5::Auth
		KF5::Completion KF5::SonnetCore KF5::SonnetUi
		KF5::Service KF5::ConfigGui KF5::ConfigCore KF5::WindowSystem
		KF5::GlobalAccel KF5::CoreAddons KF5::I18n KF5::XmlGui
		KF5::WidgetsAddons KF5::TextWidgets KF5::IconThemes
	)

		add_custom_command(
		TARGET copydlls POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:${QTLIB}>
			$<TARGET_FILE_DIR:chigraphgui>
		)
		
	install(FILES $<TARGET_FILE:${QTLIB}> DESTINATION bin)
	
	endforeach()
	
	
endif()

install(TARGETS chigraphgui DESTINATION bin)
