#pragma once

#include <fstream>
#include <vector>

#include "debug.h"

namespace LLAP {

	static std::vector<char> read_file(const std::string& file_name) {
		std::ifstream file(file_name, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			log("Failed to open file", ERROR);
		}

		size_t file_size = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(file_size);

		file.seekg(0);
		file.read(buffer.data(), file_size);

		file.close();

		return buffer;
	}

}