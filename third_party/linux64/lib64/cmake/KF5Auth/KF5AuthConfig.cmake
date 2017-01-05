
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was KF5AuthConfig.cmake.in                            ########

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

set(KAUTH_STUB_FILES_DIR "${PACKAGE_PREFIX_DIR}/share/kf5/kauth/")

set(KAUTH_BACKEND_NAME "POLKITQT5-1")
set(KAUTH_HELPER_BACKEND_NAME "DBUS")
set(KAUTH_POLICY_FILES_INSTALL_DIR "/home/russellg/chigraph/third_party/share/polkit-1/actions")
set(KAUTH_HELPER_INSTALL_DIR "lib64/libexec/kauth")
set(KAUTH_HELPER_INSTALL_ABSOLUTE_DIR "/home/russellg/chigraph/third_party/lib64/libexec/kauth")

include(CMakeFindDependencyMacro)
find_dependency(KF5CoreAddons "5.29.0")

include("${CMAKE_CURRENT_LIST_DIR}/KF5AuthTargets.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/KF5AuthMacros.cmake")

if(NOT "${KAUTH_BACKEND_NAME}" STREQUAL "FAKE")
    if(CMAKE_CROSSCOMPILING AND KF5_HOST_TOOLING)
        find_file(TARGETSFILE KF5Auth/KF5AuthToolsTargets.cmake PATHS ${KF5_HOST_TOOLING} ${CMAKE_CURRENT_LIST_DIR} NO_DEFAULT_PATH)
        include("${TARGETSFILE}")
    else()
        include("${CMAKE_CURRENT_LIST_DIR}/KF5AuthToolsTargets.cmake")
    endif()
endif()
