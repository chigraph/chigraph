
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(DEBUGGER_PRIVATE_SRCS
	debuggerplugin.cpp
	debuggerworkerthread.cpp
	breakpointview.cpp
	variableview.cpp
)

set(DEBUGGER_RESOURCE_FILES 
	chigraphdebugger.qrc
)

set(DEBUGGER_PUBLIC_SRCS
	debuggerplugin.hpp
	debuggerworkerthread.hpp
	breakpointview.hpp
	variableview.hpp
)

add_library(chigraphguidebugger STATIC ${DEBUGGER_PRIVATE_SRCS} ${DEBUGGER_PUBLIC_SRCS} ${DEBUGGER_RESOURCE_FILES})
target_link_libraries(chigraphguidebugger 
PUBLIC
	Qt5::Widgets
	KF5::I18n
	KF5::XmlGui
	KF5::TextEditor
	
	nodes

	chigraphdebugger
)

target_compile_definitions(chigraphguidebugger
PUBLIC
	-DCHIGRAPH_WITH_DEBUGGER
PRIVATE
	-DQT_STATICPLUGIN
)
