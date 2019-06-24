add_library(boost_program_options STATIC
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
target_compile_definitions(boost_program_options PUBLIC -DBOOST_ALL_NO_LIB=1)
