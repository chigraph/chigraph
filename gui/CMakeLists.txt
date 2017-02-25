set(KDE_SKIP_TEST_SETTINGS ON)

if(WIN32)
	set(GETTEXT_MSGMERGE_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/libintl-win32/bin/msgmerge.exe")
	set(GETTEXT_MSGFMT_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/libintl-win32/bin/msgfmt.exe")
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

set(GUI_SRCS
	main.cpp
	mainwindow.cpp
	chigraphnodemodel.cpp
	functionspane.cpp
	functionview.cpp
	functiondetails.cpp
	paramlistwidget.cpp
	subprocessoutputview.cpp
	modulebrowser.cpp
	moduledependencies.cpp
	execparamlistwidget.cpp
	thememanager.cpp
	localvariables.cpp
	functioninouts.cpp
	moduledetails.cpp
	typeselector.cpp
)

set(GUI_RESOURCES
	chiggui.qrc
)

set(GUI_HEADERS
	mainwindow.hpp
	chigraphnodemodel.hpp
	functionspane.hpp
	functionview.hpp
	subprocessoutputview.hpp
	paramlistwidget.hpp
	functiondetails.hpp
	modulebrowser.hpp
	moduledependencies.hpp
	execparamlistwidget.hpp
	thememanager.hpp
	localvariables.hpp
	functioninouts.hpp
	moduledetails.hpp
	typeselector.hpp
)

add_executable(chiggui ${GUI_SRCS} ${GUI_HEADERS} ${GUI_RESOURCES})

target_link_libraries(chiggui Qt5::Widgets KF5::Completion KF5::TextEditor KF5::Crash KF5::Service KF5::CoreAddons KF5::I18n KF5::Auth KF5::XmlGui KF5::WidgetsAddons KF5::TextWidgets KF5::IconThemes nodes chigcore)

target_compile_features(chiggui PRIVATE cxx_variable_templates)

if(UNIX)
        target_compile_options(chiggui PRIVATE "-fexceptions")
endif()

# copy dlls
if(WIN32)
	foreach(QTLIB Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Script Qt5::Network Qt5::DBus Qt5::PrintSupport Qt5::Xml KF5::Crash KF5::Parts KF5::KIOCore KF5::KIOWidgets KF5::JobWidgets KF5::TextEditor KF5::SyntaxHighlighting KF5::DBusAddons KF5::Archive KF5::ConfigWidgets KF5::GuiAddons KF5::ItemViews KF5::Codecs KF5::Auth KF5::Completion KF5::SonnetCore KF5::SonnetUi KF5::Service KF5::ConfigGui KF5::ConfigCore KF5::WindowSystem KF5::GlobalAccel KF5::CoreAddons KF5::I18n KF5::XmlGui KF5::WidgetsAddons KF5::TextWidgets KF5::IconThemes)

		add_custom_command(
		TARGET chiggui POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:${QTLIB}>
			$<TARGET_FILE_DIR:chiggui>
		)
	
	endforeach()
	
endif()
install(TARGETS chiggui DESTINATION bin)
