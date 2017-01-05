
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was KF5ConfigConfig.cmake.in                            ########

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

# Any changes in this ".cmake" file will be overwritten by CMake, the source is the ".cmake.in" file.

include("${CMAKE_CURRENT_LIST_DIR}/KF5ConfigTargets.cmake")

include(CMakeFindDependencyMacro)
find_dependency(Qt5Xml "5.5.0")

if(CMAKE_CROSSCOMPILING AND KF5_HOST_TOOLING)
    find_file(KCONFIGCOMPILER_PATH KF5Config/KF5ConfigCompilerTargets.cmake PATHS ${KF5_HOST_TOOLING} ${CMAKE_CURRENT_LIST_DIR} NO_DEFAULT_PATH)
    include("${KCONFIGCOMPILER_PATH}")
else()
    include("${CMAKE_CURRENT_LIST_DIR}/KF5ConfigCompilerTargets.cmake")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/KF5ConfigMacros.cmake")
