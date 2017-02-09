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
find_package(Qt5 5.6.0 REQUIRED COMPONENTS Gui Core Widgets)

find_package(KF5 REQUIRED COMPONENTS Archive DBusAddons Sonnet GuiAddons ItemViews Config ConfigWidgets Auth Codecs Completion GlobalAccel Service WindowSystem CoreAddons I18n XmlGui WidgetsAddons TextWidgets IconThemes)

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
)

add_executable(chiggui ${GUI_SRCS} ${GUI_HEADERS} ${GUI_RESOURCES})

target_link_libraries(chiggui Qt5::Widgets KF5::CoreAddons KF5::I18n KF5::Auth KF5::XmlGui KF5::WidgetsAddons KF5::TextWidgets KF5::IconThemes nodes chigcore)

target_compile_features(chiggui PRIVATE cxx_variable_templates)

if(UNIX)
        target_compile_options(chiggui PRIVATE "-fexceptions")
endif()

# copy dlls
if(WIN32)
	foreach(QTLIB Qt5::Widgets Qt5::Core Qt5::Gui KF5::DBusAddons KF5::Archive KF5::ConfigWidgets KF5::GuiAddons KF5::ItemViews KF5::Codecs KF5::Auth KF5::Completion KF5::SonnetCore KF5::SonnetUi KF5::Service KF5::ConfigGui KF5::ConfigCore KF5::WindowSystem KF5::GlobalAccel KF5::CoreAddons KF5::I18n KF5::XmlGui KF5::WidgetsAddons KF5::TextWidgets KF5::IconThemes)

		add_custom_command(
		TARGET chiggui POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:${QTLIB}>
			$<TARGET_FILE_DIR:chiggui>
		)
	
	endforeach()
	
	add_custom_command(
	TARGET chiggui POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_CURRENT_SOURCE_DIR}/../third_party/libintl-win32/bin/iconv.dll
		$<TARGET_FILE_DIR:chiggui>
	)
	
	
	add_custom_command(
	TARGET chiggui POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_CURRENT_SOURCE_DIR}/../third_party/libintl-win32/bin/libintl.dll
		$<TARGET_FILE_DIR:chiggui>
	)
	
endif()
install(TARGETS chiggui DESTINATION bin)
