/// \file TempFile.cpp

#include "chi/Support/TempFile.hpp"

namespace chi {

static char POSSIBLE_CHARS[] = "0123456789abcdefhijklmnopqrstuvwxyzABCDEFHIJKLMNOPQRSTUVWXYZ";

std::filesystem::path makeTempPath(std::string_view extension) {
	while (true) {
		// generate a random string of characters
		std::string name;
		name.reserve(10 + extension.size());

		for (int i = 0; i < 10; ++i) {
			name.push_back(POSSIBLE_CHARS[rand() % (sizeof(POSSIBLE_CHARS) - 1)]);
		}

		name.append(extension);

		auto canidate = std::filesystem::temp_directory_path() / name;
		if (!std::filesystem::exists(canidate)) { return canidate; }
	}
}

}  // namespace chi
