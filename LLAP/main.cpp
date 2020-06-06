#include "program.h"

#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include <memory>

class Triangle : public LLAP::Program {
	void init() override {};
	void loop() override {};
	void cleanup() override {};
public:
};

int main() {
	auto program = std::make_unique<Triangle>();
	
	try {
		program->run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}