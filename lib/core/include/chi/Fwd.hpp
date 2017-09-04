/// \file chi/Fwd.hpp
/// Forward declares all the chigraph data types

#ifndef CHI_FWD_HPP
#define CHI_FWD_HPP

#include "chi/Support/Fwd.hpp"

namespace chi {
struct ChiModule;
struct Context;
struct DataType;
struct NamedDataType;
struct NodeCompiler;
struct FunctionCompiler;
struct GraphFunction;
struct GraphStruct;
struct Graph;
struct GraphModule;
struct LangModule;
struct NodeInstance;
struct NodeType;
struct DataType;
struct ModuleCache;
struct PureCompiler;
}

// some basic LLVM stuff to make compiles speedy quick
namespace llvm {
class Module;
class LLVMContext;
class Type;
class DIType;
class DIBuilder;
class DICompileUnit;
class DISubprogram;
class Function;
class FunctionType;
class BasicBlock;
class IndirectBrInst;
class DebugLoc;
class Value;
struct GenericValue;
}

#endif  // CHI_FWD_HPP
