add_executable(chi main.cpp compile.cpp run.cpp interpret.cpp)

target_link_libraries(chi chigraph ${llvm_libs} boost_program_options)

install(TARGETS chi DESTINATION bin)
