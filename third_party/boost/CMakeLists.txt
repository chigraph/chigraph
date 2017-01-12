 
add_library(boost_system system/error_code.cpp)
target_include_directories(boost_system PUBLIC include)

add_library(boost_filesystem 
    filesystem/utf8_codecvt_facet.cpp
    filesystem/portability.cpp
    filesystem/operations.cpp
    filesystem/path.cpp
    filesystem/windows_file_codecvt.hpp
    filesystem/path_traits.cpp
    filesystem/codecvt_error_category.cpp
    filesystem/unique_path.cpp
    filesystem/windows_file_codecvt.cpp
)
target_include_directories(boost_filesystem PUBLIC include)
target_link_libraries(boost_filesystem PUBLIC boost_system)

add_library(boost_program_options 
    program_options/options_description.cpp
    program_options/utf8_codecvt_facet.cpp
    program_options/winmain.cpp
    program_options/split.cpp
    program_options/parsers.cpp
    program_options/config_file.cpp
    program_options/convert.cpp
    program_options/variables_map.cpp
    program_options/positional_options.cpp
    program_options/value_semantic.cpp
    program_options/cmdline.cpp
)
target_include_directories(boost_program_options PUBLIC include)
