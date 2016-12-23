
find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_AUTORCC ON)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

# find qt
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

find_package(KF5 REQUIRED COMPONENTS CoreAddons I18n XmlGui WidgetsAddons TextWidgets)

set(GUI_SRCS
	main.cpp
	mainwindow.cpp
	chignodegui.cpp
	functionspane.cpp
	functionview.cpp
        functiondetails.cpp
        paramlistwidget.cpp
        outputview.cpp
	chiggui.qrc
	modulebrowser.cpp
)

set(GUI_HEADERS
	mainwindow.hpp
	chignodegui.hpp
	functionspane.hpp
	functionview.hpp
	outputview.hpp
        paramlistwidget.hpp
        functiondetails.hpp
	modulebrowser.hpp
)

add_executable(chiggui ${GUI_SRCS} ${GUI_HEADERS})

target_link_libraries(chiggui Qt5::Widgets KF5::CoreAddons KF5::I18n KF5::XmlGui KF5::WidgetsAddons KF5::TextWidgets nodes chigcore)

target_compile_features(chiggui PRIVATE cxx_variable_templates)

if(UNIX)
        target_compile_options(chiggui PRIVATE "-fexceptions")
endif()

install(TARGETS chiggui DESTINATION bin)
