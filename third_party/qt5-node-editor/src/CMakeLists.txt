cmake_minimum_required(VERSION 2.8.8)
project(Qt5NodeEditor)

add_definitions(-std=c++11)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)

set(SRC
    graphicsbezieredge.cpp
    graphicsnode.cpp
    graphicsnodeview.cpp
    graphicsnodescene.cpp
    graphicsnodesocket.cpp
    qobjectmodel.cpp
    qmultimodeltree.cpp
    qreactiveproxymodel.cpp
    qnodeview.cpp
    qnodewidget.cpp
    qnodeeditorsocketmodel.cpp
    qmodeldatalistdecoder.cpp
    qtypecoloriserproxy.cpp
)
set(HEADERS
    graphicsbezieredge.hpp
    graphicsnode.hpp
    graphicsnodeview.hpp
    graphicsnodescene.hpp
    graphicsnodesocket.hpp
    graphicsnodedefs.hpp
)

add_library(qnodeeditor SHARED ${SRC} ${HEADERS_MOC} ${HEADERS})
qt5_use_modules(qnodeeditor Widgets)
