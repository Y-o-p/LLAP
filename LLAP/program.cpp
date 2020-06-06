#include "program.h"

namespace LLAP {

	void Program::init_window() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
	}

	void Program::create_instance() {
		VkApplicationInfo app_info{};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pApplicationName = "Triangle";
		app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.pEngineName = "No Engine";
		app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info;

		uint32_t glfw_extension_count = 0;
		const char** glfw_extensions;

		glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		// Log and copy the GLFW required extensions
		std::vector<std::string> glfw_required_extensions;
		{
			auto glfw_extensions_copy = glfw_extensions;
			log("GLFW requried extensions: ");
			for (int i = 0; i < glfw_extension_count; i++) {
				auto c_char_ptr = *glfw_extensions_copy;
				std::cout << c_char_ptr << "\n";
				glfw_required_extensions.push_back((std::string)c_char_ptr);
				glfw_extensions_copy++;
			}
		}

		create_info.enabledExtensionCount = glfw_extension_count;
		create_info.ppEnabledExtensionNames = glfw_extensions;
		create_info.enabledLayerCount = 0;

		if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
			log("Failed to create instance", ERROR);
			throw std::runtime_error("failed to create instance!");
		}
		else {
			log("Created instance");
		}

		// Check for extension support
		// Get the extension count
		uint32_t extension_count = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

		// Fill the vector with extension data
		std::vector<VkExtensionProperties> extensions(extension_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

		// Log the extensions
		std::vector<std::string> extension_names;
		log("Vulkan available extensions: ");
		for (const auto& ext : extensions) {
			std::cout << ext.extensionName << " V:" << ext.specVersion << "\n";
			extension_names.push_back((std::string)ext.extensionName);
		}

		// Check if vulkan has the extensions required
		for (const auto& ext : glfw_required_extensions) {
			auto found = std::find(extension_names.begin(), extension_names.end(), ext);
			if (found != extension_names.end()) {
				log("Found extension: " + ext);
			}
			else {
				log("Required extension not available: " + ext);
			}
		}
	}

	void Program::init_vulkan() {
		create_instance();
	}

	void Program::loop_program() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void Program::cleanup_program() {
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);

		glfwTerminate();
	}

	void Program::run() {
		init_window();
		init_vulkan();
		init();
		loop_program();
		cleanup();
		cleanup_program();

	}

}