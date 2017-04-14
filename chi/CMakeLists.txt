add_executable(chi main.cpp compile.cpp run.cpp interpret.cpp get.cpp)

target_link_libraries(chi chigraph ${llvm_libs} ${Boost_PROGRAM_OPTIONS_LIBRARY})

install(TARGETS chi DESTINATION bin)
