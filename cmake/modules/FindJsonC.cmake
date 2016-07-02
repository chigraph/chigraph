# This script finds the json-c library
# It sets up the imported target JsonC
# along with the variables:
# JSONC_INCLUDE_DIR
# JSONC_LIBRARY

find_package(PkgConfig)
pkg_check_modules(PC_JSONC QUIET JSONC)
find_path(JSONC_INCLUDE_DIR NAMES json-c/json.h HINTS ${PC_JSONC_INCLUDE_DIRS})
find_library(JSONC_LIBRARY NAMES json-c HINTS ${PC_JSONC_LIBRARY_DIRS})

add_library(JsonC SHARED IMPORTED)
set_property(TARGET JsonC PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${JSONC_INCLUDE_DIR})
set_property(TARGET JsonC PROPERTY IMPORTED_LOCATION ${JSONC_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JSONC DEFAULT_MSG JSONC_LIBRARY JSONC_INCLUDE_DIR)
