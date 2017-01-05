
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was KF5CoreAddonsConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include(CMakeFindDependencyMacro)
find_dependency(Qt5Core 5.5.0)


if(CMAKE_CROSSCOMPILING AND KF5_HOST_TOOLING)
    find_file(TARGETSFILE KF5CoreAddons/KF5CoreAddonsToolingTargets.cmake PATHS ${KF5_HOST_TOOLING} ${CMAKE_CURRENT_LIST_DIR} NO_DEFAULT_PATH)
    include("${TARGETSFILE}")
else()
    include("${CMAKE_CURRENT_LIST_DIR}/KF5CoreAddonsToolingTargets.cmake")
    if(CMAKE_CROSSCOMPILING AND DESKTOPTOJSON_EXECUTABLE)
        set_target_properties(KF5::desktoptojson PROPERTIES IMPORTED_LOCATION_NONE ${DESKTOPTOJSON_EXECUTABLE})
        set_target_properties(KF5::desktoptojson PROPERTIES IMPORTED_LOCATION ${DESKTOPTOJSON_EXECUTABLE})
    endif()
endif()
include("${CMAKE_CURRENT_LIST_DIR}/KF5CoreAddonsTargets.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/KF5CoreAddonsMacros.cmake")

