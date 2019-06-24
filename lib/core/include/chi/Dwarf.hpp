/// \file Dwarf.hpp
/// Dwarf constants not included in LLVM
/// from http://dwarfstd.org/doc/DWARF4.pdf

#include <llvm-c/DebugInfo.h>

namespace chi {

/// A dwarf encoding, see page 168
enum class DwarfEncoding : LLVMDWARFTypeEncoding {
	Address        = 0x01,
	Boolean        = 0x02,
	ComplexFloat   = 0x03,
	Float          = 0x04,
	Signed         = 0x05,
	SignedChar     = 0x06,
	Unsigned       = 0x07,
	UnsighedChar   = 0x08,
	ImaginaryFloat = 0x09,
	PackedDecimal  = 0x0a,
	NumericString  = 0x0b,
	Edited         = 0x0c,
	SignedFixed    = 0x0d,
	UnsignedFixed  = 0x0e,
};
}  // namespace chi
