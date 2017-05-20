/// \file chi/Fwd.hpp
/// Forward declares all the chigraph data types

#ifndef CHI_FWD_HPP
#define CHI_FWD_HPP

namespace chi {
struct ChiModule;
struct Context;
struct DataType;
struct NamedDataType;
struct GraphFunction;
struct GraphStruct;
struct Graph;
struct GraphModule;
struct LangModule;
struct NodeInstance;
struct NodeType;
struct Result;
struct DataType;
struct ModuleCache;
}

// some basic LLVM stuff to make compiles speedy quick
namespace llvm {
class Module;
class LLVMContext;
class Type;
class DIType;
class DIBuilder;
class DICompileUnit;
class Function;
class FunctionType;
class BasicBlock;
class DebugLoc;
class Value;
class GenericValue;
}

#endif  // CHI_FWD_HPP
