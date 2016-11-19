
find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

# find qt
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

find_package(KF5 REQUIRED COMPONENTS CoreAddons I18n XmlGui WidgetsAddons)

set(GUI_SRCS
	main.cpp
	mainwindow.cpp
)

set(GUI_HEADERS
	mainwindow.h
)

add_executable(chiggui ${GUI_SRCS} ${GUI_HEADERS})

target_link_libraries(chiggui Qt5::Widgets KF5::CoreAddons KF5::I18n KF5::XmlGui KF5::WidgetsAddons)
