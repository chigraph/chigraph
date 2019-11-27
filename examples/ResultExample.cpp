#include <chi/Support/Result.hpp>

using namespace chi;

Result thisWillFail() {
	Result res;

	res.addEntry("E122", "Huh?", {{}});

	return res;
}

int main() {
	/// [Contructing]
	Result res;
	/// [Constructing]

	/// [bValid]
	assert(res.success());
	/// [bValid]

	/// [AddWEntry]
	res.addEntry("W123", "Just a warning, don't fret.", {{"Probably here", 21}});
	/// [AddWEntry]

	/// [bValid2]
	assert(res.success());
	/// [bValid2]

	/// [AddEEntry]
	res.addEntry("E231", "Some error occured", {{"Line Number", 34}});
	/// [AddEEntry]

	/// [Check]
	assert(!res.success());
	/// [Check]

	/// [Append]
	res += thisWillFail();
	/// [Append]
}
