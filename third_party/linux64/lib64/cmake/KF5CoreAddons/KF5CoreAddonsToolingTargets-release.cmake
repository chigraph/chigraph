#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "KF5::desktoptojson" for configuration "Release"
set_property(TARGET KF5::desktoptojson APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KF5::desktoptojson PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/desktoptojson"
  )

list(APPEND _IMPORT_CHECK_TARGETS KF5::desktoptojson )
list(APPEND _IMPORT_CHECK_FILES_FOR_KF5::desktoptojson "${_IMPORT_PREFIX}/bin/desktoptojson" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
