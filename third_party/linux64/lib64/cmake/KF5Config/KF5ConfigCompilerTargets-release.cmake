#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "KF5::kconfig_compiler" for configuration "Release"
set_property(TARGET KF5::kconfig_compiler APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KF5::kconfig_compiler PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libexec/kf5/kconfig_compiler_kf5"
  )

list(APPEND _IMPORT_CHECK_TARGETS KF5::kconfig_compiler )
list(APPEND _IMPORT_CHECK_FILES_FOR_KF5::kconfig_compiler "${_IMPORT_PREFIX}/lib64/libexec/kf5/kconfig_compiler_kf5" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
