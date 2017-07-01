
add_subdirectory(chigraphsupport)
add_subdirectory(chigraph)

if (CG_BUILD_DEBUGGER)
  add_subdirectory(chigraphdebugger)
endif()
