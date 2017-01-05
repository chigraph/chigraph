#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "KF5::GlobalAccel" for configuration "Release"
set_property(TARGET KF5::GlobalAccel APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KF5::GlobalAccel PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "Qt5::X11Extras"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libKF5GlobalAccel.so.5.30.0"
  IMPORTED_SONAME_RELEASE "libKF5GlobalAccel.so.5"
  )

list(APPEND _IMPORT_CHECK_TARGETS KF5::GlobalAccel )
list(APPEND _IMPORT_CHECK_FILES_FOR_KF5::GlobalAccel "${_IMPORT_PREFIX}/lib64/libKF5GlobalAccel.so.5.30.0" )

# Import target "KF5::GlobalAccelPrivate" for configuration "Release"
set_property(TARGET KF5::GlobalAccelPrivate APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KF5::GlobalAccelPrivate PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libKF5GlobalAccelPrivate.so.5.30.0"
  IMPORTED_SONAME_RELEASE "libKF5GlobalAccelPrivate.so.5"
  )

list(APPEND _IMPORT_CHECK_TARGETS KF5::GlobalAccelPrivate )
list(APPEND _IMPORT_CHECK_FILES_FOR_KF5::GlobalAccelPrivate "${_IMPORT_PREFIX}/lib64/libKF5GlobalAccelPrivate.so.5.30.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
