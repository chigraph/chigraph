
enable_testing()

include_directories(../chig/include)

# make llvm happy
add_definitions("-D__STDC_LIMIT_MACROS")
add_definitions("-D__STDC_CONSTANT_MACROS")

add_subdirectory(MetadataParse)
add_subdirectory(JSONSerializer)
add_subdirectory(JSONReader)
