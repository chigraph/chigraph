#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "KF5::IconThemes" for configuration "Release"
set_property(TARGET KF5::IconThemes APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KF5::IconThemes PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "Qt5::DBus;Qt5::Svg;KF5::Archive;KF5::I18n;KF5::WidgetsAddons;KF5::ItemViews;KF5::ConfigWidgets;KF5::CoreAddons"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libKF5IconThemes.so.5.30.0"
  IMPORTED_SONAME_RELEASE "libKF5IconThemes.so.5"
  )

list(APPEND _IMPORT_CHECK_TARGETS KF5::IconThemes )
list(APPEND _IMPORT_CHECK_FILES_FOR_KF5::IconThemes "${_IMPORT_PREFIX}/lib64/libKF5IconThemes.so.5.30.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
