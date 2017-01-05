#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "KF5::SonnetCore" for configuration "Release"
set_property(TARGET KF5::SonnetCore APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KF5::SonnetCore PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libKF5SonnetCore.so.5.30.0"
  IMPORTED_SONAME_RELEASE "libKF5SonnetCore.so.5"
  )

list(APPEND _IMPORT_CHECK_TARGETS KF5::SonnetCore )
list(APPEND _IMPORT_CHECK_FILES_FOR_KF5::SonnetCore "${_IMPORT_PREFIX}/lib64/libKF5SonnetCore.so.5.30.0" )

# Import target "KF5::SonnetUi" for configuration "Release"
set_property(TARGET KF5::SonnetUi APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KF5::SonnetUi PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "KF5::SonnetCore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libKF5SonnetUi.so.5.30.0"
  IMPORTED_SONAME_RELEASE "libKF5SonnetUi.so.5"
  )

list(APPEND _IMPORT_CHECK_TARGETS KF5::SonnetUi )
list(APPEND _IMPORT_CHECK_FILES_FOR_KF5::SonnetUi "${_IMPORT_PREFIX}/lib64/libKF5SonnetUi.so.5.30.0" )

# Import target "KF5::parsetrigrams" for configuration "Release"
set_property(TARGET KF5::parsetrigrams APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KF5::parsetrigrams PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/parsetrigrams"
  )

list(APPEND _IMPORT_CHECK_TARGETS KF5::parsetrigrams )
list(APPEND _IMPORT_CHECK_FILES_FOR_KF5::parsetrigrams "${_IMPORT_PREFIX}/bin/parsetrigrams" )

# Import target "KF5::gentrigrams" for configuration "Release"
set_property(TARGET KF5::gentrigrams APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KF5::gentrigrams PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/gentrigrams"
  )

list(APPEND _IMPORT_CHECK_TARGETS KF5::gentrigrams )
list(APPEND _IMPORT_CHECK_FILES_FOR_KF5::gentrigrams "${_IMPORT_PREFIX}/bin/gentrigrams" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
