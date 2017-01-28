set(KDE_SKIP_TEST_SETTINGS ON)
find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_AUTORCC ON)

include(KDEInstallDirs)
include(KDECMakeSettings)
#include(KDECompilerSettings)
include(FeatureSummary)

# find qt
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

find_package(KF5 REQUIRED COMPONENTS CoreAddons I18n XmlGui WidgetsAddons TextWidgets)

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
)

add_executable(chiggui ${GUI_SRCS} ${GUI_HEADERS} ${GUI_RESOURCES})

target_link_libraries(chiggui Qt5::Widgets KF5::CoreAddons KF5::I18n KF5::XmlGui KF5::WidgetsAddons KF5::TextWidgets nodes chigcore)

target_compile_features(chiggui PRIVATE cxx_variable_templates)

if(UNIX)
        target_compile_options(chiggui PRIVATE "-fexceptions")
endif()

install(TARGETS chiggui DESTINATION bin)
