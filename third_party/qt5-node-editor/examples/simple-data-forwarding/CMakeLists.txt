cmake_minimum_required(VERSION 2.8.8)
project(simple-data-forwarding)

add_definitions(-std=c++11) 
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)

set(SRC
	main.cpp
	mainwindow.cpp
	test_qobjects.cpp
)
set(HEADERS
	mainwindow.hpp
	test_qobjects.hpp
)

add_executable(${PROJECT_NAME} ${SRC} ${HEADERS_MOC} ${HEADERS})
target_link_libraries(${PROJECT_NAME}
	Qt5NodeEditor
)
qt5_use_modules(${PROJECT_NAME} Widgets)
