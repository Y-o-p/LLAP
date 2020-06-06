#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <string>

#include "debug.h"

namespace LLAP {

	class Program {
	protected:
		GLFWwindow* window;

	private:
		VkInstance instance;

		void create_instance();
		void init_vulkan();
		
		void init_window();
		void cleanup_program();
		void loop_program();

		virtual void init() = 0;
		virtual void loop() = 0;
		virtual void cleanup() = 0;

	public:
		void run();
	};

}