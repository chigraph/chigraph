#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LLVMSupport" for configuration "Release"
set_property(TARGET LLVMSupport APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMSupport PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMSupport.so"
  IMPORTED_SONAME_RELEASE "libLLVMSupport.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMSupport )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMSupport "${_IMPORT_PREFIX}/lib/libLLVMSupport.so" )

# Import target "LLVMTableGen" for configuration "Release"
set_property(TARGET LLVMTableGen APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMTableGen PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMTableGen.so"
  IMPORTED_SONAME_RELEASE "libLLVMTableGen.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMTableGen )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMTableGen "${_IMPORT_PREFIX}/lib/libLLVMTableGen.so" )

# Import target "llvm-tblgen" for configuration "Release"
set_property(TARGET llvm-tblgen APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-tblgen PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-tblgen"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-tblgen )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-tblgen "${_IMPORT_PREFIX}/bin/llvm-tblgen" )

# Import target "LLVMCore" for configuration "Release"
set_property(TARGET LLVMCore APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMCore PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMCore.so"
  IMPORTED_SONAME_RELEASE "libLLVMCore.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMCore )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMCore "${_IMPORT_PREFIX}/lib/libLLVMCore.so" )

# Import target "LLVMIRReader" for configuration "Release"
set_property(TARGET LLVMIRReader APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMIRReader PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMIRReader.so"
  IMPORTED_SONAME_RELEASE "libLLVMIRReader.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMIRReader )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMIRReader "${_IMPORT_PREFIX}/lib/libLLVMIRReader.so" )

# Import target "LLVMCodeGen" for configuration "Release"
set_property(TARGET LLVMCodeGen APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMCodeGen PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMCodeGen.so"
  IMPORTED_SONAME_RELEASE "libLLVMCodeGen.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMCodeGen )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMCodeGen "${_IMPORT_PREFIX}/lib/libLLVMCodeGen.so" )

# Import target "LLVMSelectionDAG" for configuration "Release"
set_property(TARGET LLVMSelectionDAG APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMSelectionDAG PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMSelectionDAG.so"
  IMPORTED_SONAME_RELEASE "libLLVMSelectionDAG.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMSelectionDAG )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMSelectionDAG "${_IMPORT_PREFIX}/lib/libLLVMSelectionDAG.so" )

# Import target "LLVMAsmPrinter" for configuration "Release"
set_property(TARGET LLVMAsmPrinter APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMAsmPrinter PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMAsmPrinter.so"
  IMPORTED_SONAME_RELEASE "libLLVMAsmPrinter.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMAsmPrinter )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMAsmPrinter "${_IMPORT_PREFIX}/lib/libLLVMAsmPrinter.so" )

# Import target "LLVMMIRParser" for configuration "Release"
set_property(TARGET LLVMMIRParser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMMIRParser PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMMIRParser.so"
  IMPORTED_SONAME_RELEASE "libLLVMMIRParser.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMMIRParser )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMMIRParser "${_IMPORT_PREFIX}/lib/libLLVMMIRParser.so" )

# Import target "LLVMGlobalISel" for configuration "Release"
set_property(TARGET LLVMGlobalISel APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMGlobalISel PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMGlobalISel.so"
  IMPORTED_SONAME_RELEASE "libLLVMGlobalISel.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMGlobalISel )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMGlobalISel "${_IMPORT_PREFIX}/lib/libLLVMGlobalISel.so" )

# Import target "LLVMBitReader" for configuration "Release"
set_property(TARGET LLVMBitReader APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMBitReader PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMBitReader.so"
  IMPORTED_SONAME_RELEASE "libLLVMBitReader.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMBitReader )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMBitReader "${_IMPORT_PREFIX}/lib/libLLVMBitReader.so" )

# Import target "LLVMBitWriter" for configuration "Release"
set_property(TARGET LLVMBitWriter APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMBitWriter PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMBitWriter.so"
  IMPORTED_SONAME_RELEASE "libLLVMBitWriter.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMBitWriter )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMBitWriter "${_IMPORT_PREFIX}/lib/libLLVMBitWriter.so" )

# Import target "LLVMTransformUtils" for configuration "Release"
set_property(TARGET LLVMTransformUtils APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMTransformUtils PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMTransformUtils.so"
  IMPORTED_SONAME_RELEASE "libLLVMTransformUtils.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMTransformUtils )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMTransformUtils "${_IMPORT_PREFIX}/lib/libLLVMTransformUtils.so" )

# Import target "LLVMInstrumentation" for configuration "Release"
set_property(TARGET LLVMInstrumentation APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMInstrumentation PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMInstrumentation.so"
  IMPORTED_SONAME_RELEASE "libLLVMInstrumentation.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMInstrumentation )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMInstrumentation "${_IMPORT_PREFIX}/lib/libLLVMInstrumentation.so" )

# Import target "LLVMInstCombine" for configuration "Release"
set_property(TARGET LLVMInstCombine APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMInstCombine PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMInstCombine.so"
  IMPORTED_SONAME_RELEASE "libLLVMInstCombine.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMInstCombine )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMInstCombine "${_IMPORT_PREFIX}/lib/libLLVMInstCombine.so" )

# Import target "LLVMScalarOpts" for configuration "Release"
set_property(TARGET LLVMScalarOpts APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMScalarOpts PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMScalarOpts.so"
  IMPORTED_SONAME_RELEASE "libLLVMScalarOpts.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMScalarOpts )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMScalarOpts "${_IMPORT_PREFIX}/lib/libLLVMScalarOpts.so" )

# Import target "LLVMipo" for configuration "Release"
set_property(TARGET LLVMipo APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMipo PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMipo.so"
  IMPORTED_SONAME_RELEASE "libLLVMipo.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMipo )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMipo "${_IMPORT_PREFIX}/lib/libLLVMipo.so" )

# Import target "LLVMVectorize" for configuration "Release"
set_property(TARGET LLVMVectorize APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMVectorize PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMVectorize.so"
  IMPORTED_SONAME_RELEASE "libLLVMVectorize.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMVectorize )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMVectorize "${_IMPORT_PREFIX}/lib/libLLVMVectorize.so" )

# Import target "LLVMHello" for configuration "Release"
set_property(TARGET LLVMHello APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMHello PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/LLVMHello.so"
  IMPORTED_NO_SONAME_RELEASE "TRUE"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMHello )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMHello "${_IMPORT_PREFIX}/lib/LLVMHello.so" )

# Import target "LLVMObjCARCOpts" for configuration "Release"
set_property(TARGET LLVMObjCARCOpts APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMObjCARCOpts PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMObjCARCOpts.so"
  IMPORTED_SONAME_RELEASE "libLLVMObjCARCOpts.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMObjCARCOpts )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMObjCARCOpts "${_IMPORT_PREFIX}/lib/libLLVMObjCARCOpts.so" )

# Import target "LLVMLinker" for configuration "Release"
set_property(TARGET LLVMLinker APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMLinker PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMLinker.so"
  IMPORTED_SONAME_RELEASE "libLLVMLinker.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMLinker )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMLinker "${_IMPORT_PREFIX}/lib/libLLVMLinker.so" )

# Import target "LLVMAnalysis" for configuration "Release"
set_property(TARGET LLVMAnalysis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMAnalysis PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMAnalysis.so"
  IMPORTED_SONAME_RELEASE "libLLVMAnalysis.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMAnalysis )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMAnalysis "${_IMPORT_PREFIX}/lib/libLLVMAnalysis.so" )

# Import target "LLVMLTO" for configuration "Release"
set_property(TARGET LLVMLTO APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMLTO PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMLTO.so"
  IMPORTED_SONAME_RELEASE "libLLVMLTO.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMLTO )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMLTO "${_IMPORT_PREFIX}/lib/libLLVMLTO.so" )

# Import target "LLVMMC" for configuration "Release"
set_property(TARGET LLVMMC APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMMC PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMMC.so"
  IMPORTED_SONAME_RELEASE "libLLVMMC.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMMC )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMMC "${_IMPORT_PREFIX}/lib/libLLVMMC.so" )

# Import target "LLVMMCParser" for configuration "Release"
set_property(TARGET LLVMMCParser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMMCParser PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMMCParser.so"
  IMPORTED_SONAME_RELEASE "libLLVMMCParser.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMMCParser )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMMCParser "${_IMPORT_PREFIX}/lib/libLLVMMCParser.so" )

# Import target "LLVMMCDisassembler" for configuration "Release"
set_property(TARGET LLVMMCDisassembler APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMMCDisassembler PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMMCDisassembler.so"
  IMPORTED_SONAME_RELEASE "libLLVMMCDisassembler.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMMCDisassembler )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMMCDisassembler "${_IMPORT_PREFIX}/lib/libLLVMMCDisassembler.so" )

# Import target "LLVMObject" for configuration "Release"
set_property(TARGET LLVMObject APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMObject PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMObject.so"
  IMPORTED_SONAME_RELEASE "libLLVMObject.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMObject )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMObject "${_IMPORT_PREFIX}/lib/libLLVMObject.so" )

# Import target "LLVMObjectYAML" for configuration "Release"
set_property(TARGET LLVMObjectYAML APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMObjectYAML PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMObjectYAML.so"
  IMPORTED_SONAME_RELEASE "libLLVMObjectYAML.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMObjectYAML )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMObjectYAML "${_IMPORT_PREFIX}/lib/libLLVMObjectYAML.so" )

# Import target "LLVMOption" for configuration "Release"
set_property(TARGET LLVMOption APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMOption PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMOption.so"
  IMPORTED_SONAME_RELEASE "libLLVMOption.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMOption )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMOption "${_IMPORT_PREFIX}/lib/libLLVMOption.so" )

# Import target "LLVMDebugInfoCodeView" for configuration "Release"
set_property(TARGET LLVMDebugInfoCodeView APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMDebugInfoCodeView PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMDebugInfoCodeView.so"
  IMPORTED_SONAME_RELEASE "libLLVMDebugInfoCodeView.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMDebugInfoCodeView )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMDebugInfoCodeView "${_IMPORT_PREFIX}/lib/libLLVMDebugInfoCodeView.so" )

# Import target "LLVMDebugInfoDWARF" for configuration "Release"
set_property(TARGET LLVMDebugInfoDWARF APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMDebugInfoDWARF PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMDebugInfoDWARF.so"
  IMPORTED_SONAME_RELEASE "libLLVMDebugInfoDWARF.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMDebugInfoDWARF )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMDebugInfoDWARF "${_IMPORT_PREFIX}/lib/libLLVMDebugInfoDWARF.so" )

# Import target "LLVMDebugInfoPDB" for configuration "Release"
set_property(TARGET LLVMDebugInfoPDB APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMDebugInfoPDB PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMDebugInfoPDB.so"
  IMPORTED_SONAME_RELEASE "libLLVMDebugInfoPDB.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMDebugInfoPDB )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMDebugInfoPDB "${_IMPORT_PREFIX}/lib/libLLVMDebugInfoPDB.so" )

# Import target "LLVMSymbolize" for configuration "Release"
set_property(TARGET LLVMSymbolize APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMSymbolize PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMSymbolize.so"
  IMPORTED_SONAME_RELEASE "libLLVMSymbolize.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMSymbolize )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMSymbolize "${_IMPORT_PREFIX}/lib/libLLVMSymbolize.so" )

# Import target "LLVMExecutionEngine" for configuration "Release"
set_property(TARGET LLVMExecutionEngine APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMExecutionEngine PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMExecutionEngine.so"
  IMPORTED_SONAME_RELEASE "libLLVMExecutionEngine.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMExecutionEngine )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMExecutionEngine "${_IMPORT_PREFIX}/lib/libLLVMExecutionEngine.so" )

# Import target "LLVMInterpreter" for configuration "Release"
set_property(TARGET LLVMInterpreter APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMInterpreter PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMInterpreter.so"
  IMPORTED_SONAME_RELEASE "libLLVMInterpreter.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMInterpreter )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMInterpreter "${_IMPORT_PREFIX}/lib/libLLVMInterpreter.so" )

# Import target "LLVMMCJIT" for configuration "Release"
set_property(TARGET LLVMMCJIT APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMMCJIT PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMMCJIT.so"
  IMPORTED_SONAME_RELEASE "libLLVMMCJIT.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMMCJIT )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMMCJIT "${_IMPORT_PREFIX}/lib/libLLVMMCJIT.so" )

# Import target "LLVMOrcJIT" for configuration "Release"
set_property(TARGET LLVMOrcJIT APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMOrcJIT PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMOrcJIT.so"
  IMPORTED_SONAME_RELEASE "libLLVMOrcJIT.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMOrcJIT )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMOrcJIT "${_IMPORT_PREFIX}/lib/libLLVMOrcJIT.so" )

# Import target "LLVMRuntimeDyld" for configuration "Release"
set_property(TARGET LLVMRuntimeDyld APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMRuntimeDyld PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMRuntimeDyld.so"
  IMPORTED_SONAME_RELEASE "libLLVMRuntimeDyld.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMRuntimeDyld )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMRuntimeDyld "${_IMPORT_PREFIX}/lib/libLLVMRuntimeDyld.so" )

# Import target "LLVMTarget" for configuration "Release"
set_property(TARGET LLVMTarget APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMTarget PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMTarget.so"
  IMPORTED_SONAME_RELEASE "libLLVMTarget.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMTarget )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMTarget "${_IMPORT_PREFIX}/lib/libLLVMTarget.so" )

# Import target "LLVMX86CodeGen" for configuration "Release"
set_property(TARGET LLVMX86CodeGen APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMX86CodeGen PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMX86CodeGen.so"
  IMPORTED_SONAME_RELEASE "libLLVMX86CodeGen.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMX86CodeGen )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMX86CodeGen "${_IMPORT_PREFIX}/lib/libLLVMX86CodeGen.so" )

# Import target "LLVMX86AsmParser" for configuration "Release"
set_property(TARGET LLVMX86AsmParser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMX86AsmParser PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMX86AsmParser.so"
  IMPORTED_SONAME_RELEASE "libLLVMX86AsmParser.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMX86AsmParser )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMX86AsmParser "${_IMPORT_PREFIX}/lib/libLLVMX86AsmParser.so" )

# Import target "LLVMX86Disassembler" for configuration "Release"
set_property(TARGET LLVMX86Disassembler APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMX86Disassembler PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMX86Disassembler.so"
  IMPORTED_SONAME_RELEASE "libLLVMX86Disassembler.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMX86Disassembler )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMX86Disassembler "${_IMPORT_PREFIX}/lib/libLLVMX86Disassembler.so" )

# Import target "LLVMX86AsmPrinter" for configuration "Release"
set_property(TARGET LLVMX86AsmPrinter APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMX86AsmPrinter PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMX86AsmPrinter.so"
  IMPORTED_SONAME_RELEASE "libLLVMX86AsmPrinter.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMX86AsmPrinter )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMX86AsmPrinter "${_IMPORT_PREFIX}/lib/libLLVMX86AsmPrinter.so" )

# Import target "LLVMX86Desc" for configuration "Release"
set_property(TARGET LLVMX86Desc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMX86Desc PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMX86Desc.so"
  IMPORTED_SONAME_RELEASE "libLLVMX86Desc.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMX86Desc )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMX86Desc "${_IMPORT_PREFIX}/lib/libLLVMX86Desc.so" )

# Import target "LLVMX86Info" for configuration "Release"
set_property(TARGET LLVMX86Info APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMX86Info PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMX86Info.so"
  IMPORTED_SONAME_RELEASE "libLLVMX86Info.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMX86Info )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMX86Info "${_IMPORT_PREFIX}/lib/libLLVMX86Info.so" )

# Import target "LLVMX86Utils" for configuration "Release"
set_property(TARGET LLVMX86Utils APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMX86Utils PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMX86Utils.so"
  IMPORTED_SONAME_RELEASE "libLLVMX86Utils.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMX86Utils )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMX86Utils "${_IMPORT_PREFIX}/lib/libLLVMX86Utils.so" )

# Import target "LLVMAsmParser" for configuration "Release"
set_property(TARGET LLVMAsmParser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMAsmParser PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMAsmParser.so"
  IMPORTED_SONAME_RELEASE "libLLVMAsmParser.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMAsmParser )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMAsmParser "${_IMPORT_PREFIX}/lib/libLLVMAsmParser.so" )

# Import target "LLVMLineEditor" for configuration "Release"
set_property(TARGET LLVMLineEditor APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMLineEditor PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMLineEditor.so"
  IMPORTED_SONAME_RELEASE "libLLVMLineEditor.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMLineEditor )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMLineEditor "${_IMPORT_PREFIX}/lib/libLLVMLineEditor.so" )

# Import target "LLVMProfileData" for configuration "Release"
set_property(TARGET LLVMProfileData APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMProfileData PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMProfileData.so"
  IMPORTED_SONAME_RELEASE "libLLVMProfileData.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMProfileData )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMProfileData "${_IMPORT_PREFIX}/lib/libLLVMProfileData.so" )

# Import target "LLVMCoverage" for configuration "Release"
set_property(TARGET LLVMCoverage APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMCoverage PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMCoverage.so"
  IMPORTED_SONAME_RELEASE "libLLVMCoverage.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMCoverage )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMCoverage "${_IMPORT_PREFIX}/lib/libLLVMCoverage.so" )

# Import target "LLVMPasses" for configuration "Release"
set_property(TARGET LLVMPasses APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMPasses PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMPasses.so"
  IMPORTED_SONAME_RELEASE "libLLVMPasses.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMPasses )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMPasses "${_IMPORT_PREFIX}/lib/libLLVMPasses.so" )

# Import target "LLVMLibDriver" for configuration "Release"
set_property(TARGET LLVMLibDriver APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LLVMLibDriver PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLLVMLibDriver.so"
  IMPORTED_SONAME_RELEASE "libLLVMLibDriver.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLVMLibDriver )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLVMLibDriver "${_IMPORT_PREFIX}/lib/libLLVMLibDriver.so" )

# Import target "LTO" for configuration "Release"
set_property(TARGET LTO APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LTO PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libLTO.so"
  IMPORTED_SONAME_RELEASE "libLTO.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS LTO )
list(APPEND _IMPORT_CHECK_FILES_FOR_LTO "${_IMPORT_PREFIX}/lib/libLTO.so" )

# Import target "llvm-ar" for configuration "Release"
set_property(TARGET llvm-ar APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-ar PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-ar"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-ar )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-ar "${_IMPORT_PREFIX}/bin/llvm-ar" )

# Import target "llvm-config" for configuration "Release"
set_property(TARGET llvm-config APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-config PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-config"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-config )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-config "${_IMPORT_PREFIX}/bin/llvm-config" )

# Import target "llvm-lto" for configuration "Release"
set_property(TARGET llvm-lto APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-lto PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-lto"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-lto )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-lto "${_IMPORT_PREFIX}/bin/llvm-lto" )

# Import target "llvm-profdata" for configuration "Release"
set_property(TARGET llvm-profdata APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-profdata PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-profdata"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-profdata )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-profdata "${_IMPORT_PREFIX}/bin/llvm-profdata" )

# Import target "bugpoint" for configuration "Release"
set_property(TARGET bugpoint APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(bugpoint PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/bugpoint"
  )

list(APPEND _IMPORT_CHECK_TARGETS bugpoint )
list(APPEND _IMPORT_CHECK_FILES_FOR_bugpoint "${_IMPORT_PREFIX}/bin/bugpoint" )

# Import target "BugpointPasses" for configuration "Release"
set_property(TARGET BugpointPasses APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(BugpointPasses PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/BugpointPasses.so"
  IMPORTED_NO_SONAME_RELEASE "TRUE"
  )

list(APPEND _IMPORT_CHECK_TARGETS BugpointPasses )
list(APPEND _IMPORT_CHECK_FILES_FOR_BugpointPasses "${_IMPORT_PREFIX}/lib/BugpointPasses.so" )

# Import target "llvm-dsymutil" for configuration "Release"
set_property(TARGET llvm-dsymutil APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-dsymutil PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-dsymutil"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-dsymutil )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-dsymutil "${_IMPORT_PREFIX}/bin/llvm-dsymutil" )

# Import target "llc" for configuration "Release"
set_property(TARGET llc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llc PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llc"
  )

list(APPEND _IMPORT_CHECK_TARGETS llc )
list(APPEND _IMPORT_CHECK_FILES_FOR_llc "${_IMPORT_PREFIX}/bin/llc" )

# Import target "lli" for configuration "Release"
set_property(TARGET lli APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(lli PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/lli"
  )

list(APPEND _IMPORT_CHECK_TARGETS lli )
list(APPEND _IMPORT_CHECK_FILES_FOR_lli "${_IMPORT_PREFIX}/bin/lli" )

# Import target "llvm-as" for configuration "Release"
set_property(TARGET llvm-as APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-as PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-as"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-as )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-as "${_IMPORT_PREFIX}/bin/llvm-as" )

# Import target "llvm-bcanalyzer" for configuration "Release"
set_property(TARGET llvm-bcanalyzer APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-bcanalyzer PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-bcanalyzer"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-bcanalyzer )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-bcanalyzer "${_IMPORT_PREFIX}/bin/llvm-bcanalyzer" )

# Import target "llvm-c-test" for configuration "Release"
set_property(TARGET llvm-c-test APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-c-test PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-c-test"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-c-test )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-c-test "${_IMPORT_PREFIX}/bin/llvm-c-test" )

# Import target "llvm-cov" for configuration "Release"
set_property(TARGET llvm-cov APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-cov PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-cov"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-cov )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-cov "${_IMPORT_PREFIX}/bin/llvm-cov" )

# Import target "llvm-cxxdump" for configuration "Release"
set_property(TARGET llvm-cxxdump APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-cxxdump PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-cxxdump"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-cxxdump )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-cxxdump "${_IMPORT_PREFIX}/bin/llvm-cxxdump" )

# Import target "llvm-diff" for configuration "Release"
set_property(TARGET llvm-diff APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-diff PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-diff"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-diff )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-diff "${_IMPORT_PREFIX}/bin/llvm-diff" )

# Import target "llvm-dis" for configuration "Release"
set_property(TARGET llvm-dis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-dis PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-dis"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-dis )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-dis "${_IMPORT_PREFIX}/bin/llvm-dis" )

# Import target "llvm-dwarfdump" for configuration "Release"
set_property(TARGET llvm-dwarfdump APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-dwarfdump PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-dwarfdump"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-dwarfdump )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-dwarfdump "${_IMPORT_PREFIX}/bin/llvm-dwarfdump" )

# Import target "llvm-dwp" for configuration "Release"
set_property(TARGET llvm-dwp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-dwp PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-dwp"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-dwp )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-dwp "${_IMPORT_PREFIX}/bin/llvm-dwp" )

# Import target "llvm-extract" for configuration "Release"
set_property(TARGET llvm-extract APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-extract PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-extract"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-extract )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-extract "${_IMPORT_PREFIX}/bin/llvm-extract" )

# Import target "llvm-link" for configuration "Release"
set_property(TARGET llvm-link APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-link PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-link"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-link )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-link "${_IMPORT_PREFIX}/bin/llvm-link" )

# Import target "llvm-mc" for configuration "Release"
set_property(TARGET llvm-mc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-mc PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-mc"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-mc )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-mc "${_IMPORT_PREFIX}/bin/llvm-mc" )

# Import target "llvm-mcmarkup" for configuration "Release"
set_property(TARGET llvm-mcmarkup APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-mcmarkup PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-mcmarkup"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-mcmarkup )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-mcmarkup "${_IMPORT_PREFIX}/bin/llvm-mcmarkup" )

# Import target "llvm-nm" for configuration "Release"
set_property(TARGET llvm-nm APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-nm PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-nm"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-nm )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-nm "${_IMPORT_PREFIX}/bin/llvm-nm" )

# Import target "llvm-objdump" for configuration "Release"
set_property(TARGET llvm-objdump APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-objdump PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-objdump"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-objdump )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-objdump "${_IMPORT_PREFIX}/bin/llvm-objdump" )

# Import target "llvm-pdbdump" for configuration "Release"
set_property(TARGET llvm-pdbdump APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-pdbdump PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-pdbdump"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-pdbdump )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-pdbdump "${_IMPORT_PREFIX}/bin/llvm-pdbdump" )

# Import target "llvm-readobj" for configuration "Release"
set_property(TARGET llvm-readobj APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-readobj PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-readobj"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-readobj )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-readobj "${_IMPORT_PREFIX}/bin/llvm-readobj" )

# Import target "llvm-rtdyld" for configuration "Release"
set_property(TARGET llvm-rtdyld APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-rtdyld PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-rtdyld"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-rtdyld )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-rtdyld "${_IMPORT_PREFIX}/bin/llvm-rtdyld" )

# Import target "llvm-size" for configuration "Release"
set_property(TARGET llvm-size APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-size PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-size"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-size )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-size "${_IMPORT_PREFIX}/bin/llvm-size" )

# Import target "llvm-split" for configuration "Release"
set_property(TARGET llvm-split APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-split PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-split"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-split )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-split "${_IMPORT_PREFIX}/bin/llvm-split" )

# Import target "llvm-stress" for configuration "Release"
set_property(TARGET llvm-stress APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-stress PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-stress"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-stress )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-stress "${_IMPORT_PREFIX}/bin/llvm-stress" )

# Import target "llvm-symbolizer" for configuration "Release"
set_property(TARGET llvm-symbolizer APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-symbolizer PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/llvm-symbolizer"
  )

list(APPEND _IMPORT_CHECK_TARGETS llvm-symbolizer )
list(APPEND _IMPORT_CHECK_FILES_FOR_llvm-symbolizer "${_IMPORT_PREFIX}/bin/llvm-symbolizer" )

# Import target "obj2yaml" for configuration "Release"
set_property(TARGET obj2yaml APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(obj2yaml PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/obj2yaml"
  )

list(APPEND _IMPORT_CHECK_TARGETS obj2yaml )
list(APPEND _IMPORT_CHECK_FILES_FOR_obj2yaml "${_IMPORT_PREFIX}/bin/obj2yaml" )

# Import target "opt" for configuration "Release"
set_property(TARGET opt APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opt PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/opt"
  )

list(APPEND _IMPORT_CHECK_TARGETS opt )
list(APPEND _IMPORT_CHECK_FILES_FOR_opt "${_IMPORT_PREFIX}/bin/opt" )

# Import target "sancov" for configuration "Release"
set_property(TARGET sancov APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(sancov PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/sancov"
  )

list(APPEND _IMPORT_CHECK_TARGETS sancov )
list(APPEND _IMPORT_CHECK_FILES_FOR_sancov "${_IMPORT_PREFIX}/bin/sancov" )

# Import target "sanstats" for configuration "Release"
set_property(TARGET sanstats APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(sanstats PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/sanstats"
  )

list(APPEND _IMPORT_CHECK_TARGETS sanstats )
list(APPEND _IMPORT_CHECK_FILES_FOR_sanstats "${_IMPORT_PREFIX}/bin/sanstats" )

# Import target "verify-uselistorder" for configuration "Release"
set_property(TARGET verify-uselistorder APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(verify-uselistorder PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/verify-uselistorder"
  )

list(APPEND _IMPORT_CHECK_TARGETS verify-uselistorder )
list(APPEND _IMPORT_CHECK_FILES_FOR_verify-uselistorder "${_IMPORT_PREFIX}/bin/verify-uselistorder" )

# Import target "yaml2obj" for configuration "Release"
set_property(TARGET yaml2obj APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(yaml2obj PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/yaml2obj"
  )

list(APPEND _IMPORT_CHECK_TARGETS yaml2obj )
list(APPEND _IMPORT_CHECK_FILES_FOR_yaml2obj "${_IMPORT_PREFIX}/bin/yaml2obj" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
