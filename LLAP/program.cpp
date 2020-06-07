#include "program.h"

namespace LLAP {

	void Program::init_window() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
	}

	bool Program::check_validation_support() {
		uint32_t layer_count;
		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

		std::vector<VkLayerProperties> available_layers(layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

		// Log the available layers
		std::vector<std::string> layer_names;
		log("Available layers:");
		for (const auto& layer : available_layers) {
			std::cout << layer.layerName << " V:" << layer.specVersion << "\n";
			layer_names.push_back(layer.layerName);
		}

		// Check for required validation layers
		for (const auto& layer : validation_layers) {
			auto layer_s = std::string(layer);
			if (std::find(layer_names.begin(), layer_names.end(), layer_s) != layer_names.end()) {
				log("Found layer: " + layer_s);
			}
			else {
				log("Layer not available: " + layer_s, ERROR);
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> Program::get_required_extensions() {
		uint32_t glfw_extension_count = 0;
		const char** glfw_extensions;
		glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
		std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

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

		// Check for extension support
		// Get the extension count
		uint32_t extension_count = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

		// Fill the vector with extension data
		std::vector<VkExtensionProperties> vk_extensions(extension_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, vk_extensions.data());

		// Log the extensions
		std::vector<std::string> extension_names;
		log("Vulkan available extensions: ");
		for (const auto& ext : vk_extensions) {
			std::cout << ext.extensionName << " V:" << ext.specVersion << "\n";
			extension_names.push_back(static_cast<std::string>(ext.extensionName));
		}

		// Check if vulkan has the extensions required
		for (const auto& ext : glfw_required_extensions) {
			if (std::find(extension_names.begin(), extension_names.end(), ext) != extension_names.end()) {
				log("Found extension: " + ext);
			}
			else {
				log("Required extension not available: " + ext);
			}
		}

		if (enable_validation_layers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL Program::debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, 
		VkDebugUtilsMessageTypeFlagsEXT message_type, 
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data, 
		void* user_data)
	{
		std::cerr << "validation layer: " << callback_data->pMessage << "\n";

		return VK_FALSE;
	}

	VkResult Program::create_debug_utils_messenger_EXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* create_info,
		const VkAllocationCallbacks* allocator,
		VkDebugUtilsMessengerEXT* debug_messenger) 
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, create_info, allocator, debug_messenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void Program::destroy_debug_utils_messenger_EXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debug_messenger,
		const VkAllocationCallbacks* allocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debug_messenger, allocator);
		}
	}

	void Program::pick_gpu() {
		// Query the number of graphics cards
		uint32_t device_count = 0;
		vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

		if (device_count == 0) {
			log("No GPU with Vulkan support was found", ERROR);
		}

		std::vector<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

		log("Querying devices for suitable GPU:");
		for (const auto& device : devices) {
			if (is_gpu_suitable(device)) {
				physical_device = device;
				break;
			}
		}

		if (physical_device == VK_NULL_HANDLE) {
			log("No GPU with Vulkan support is suitable", ERROR);
		}
	}

	bool Program::is_gpu_suitable(VkPhysicalDevice device) {
		VkPhysicalDeviceProperties device_properties;
		VkPhysicalDeviceFeatures device_features;
		vkGetPhysicalDeviceProperties(device, &device_properties);
		vkGetPhysicalDeviceFeatures(device, &device_features);

		std::cout << device_properties.deviceName << "\n";

		QueueFamilyIndices indices = find_queue_families(device);
		if (indices.graphics_family.has_value()) {
			log("Device [" + static_cast<std::string>(device_properties.deviceName) + "] is suitable");
			return true;
		}
	}

	void Program::create_logical_device() {
		QueueFamilyIndices indices = find_queue_families(physical_device);
		
		VkDeviceQueueCreateInfo queue_create_info{};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = indices.graphics_family.value();
		queue_create_info.queueCount = 1;
		float queue_priority = 1.0f;
		queue_create_info.pQueuePriorities = &queue_priority;

		VkPhysicalDeviceFeatures device_features{};

		VkDeviceCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.pQueueCreateInfos = &queue_create_info;
		create_info.queueCreateInfoCount = 1;
		create_info.pEnabledFeatures = &device_features;

		create_info.enabledExtensionCount = 0;

		if (enable_validation_layers) {
			create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
			create_info.ppEnabledLayerNames = validation_layers.data();
		}
		else {
			create_info.enabledLayerCount = 0;
		}

		if (vkCreateDevice(physical_device, &create_info, nullptr, &device) != VK_SUCCESS) {
			log("Failed to create a logical device", ERROR);
		}

		log("Created logical device");

		vkGetDeviceQueue(device, indices.graphics_family.value(), 0, &graphics_queue);
	}

	QueueFamilyIndices Program::find_queue_families(VkPhysicalDevice device) {
		QueueFamilyIndices indices;
		
		uint32_t queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

		std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

		int i = 0;
		for (const auto& queue_family : queue_families) {
			if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphics_family = i;
			}

			i++;
		}

		return indices;
	}

	void Program::create_instance() {
		// Check for validation layers
		if (enable_validation_layers && !check_validation_support()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

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

		// Get required extensions
		auto extensions = get_required_extensions();
		create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		create_info.ppEnabledExtensionNames = extensions.data();

		// Layers
		VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
		if (enable_validation_layers) {
			create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
			create_info.ppEnabledLayerNames = validation_layers.data();

			populate_debug_messenger_create_info(debug_create_info);
			create_info.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debug_create_info);
		}
		else {
			create_info.enabledLayerCount = 0;
			create_info.pNext = nullptr;
		}

		if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
			log("Failed to create instance", ERROR);
			throw std::runtime_error("failed to create instance!");
		}
		else {
			log("Created instance");
		}
	}

	void Program::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
		create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		create_info.pfnUserCallback = debug_callback;
		create_info.pUserData = nullptr; // Optional
	}

	void Program::setup_debug_messenger() {
		if (!enable_validation_layers) return;
		
		VkDebugUtilsMessengerCreateInfoEXT create_info{};
		populate_debug_messenger_create_info(create_info);

		if (create_debug_utils_messenger_EXT(instance, &create_info, nullptr, &debug_messenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	void Program::init_vulkan() {
		create_instance();
		setup_debug_messenger();
		pick_gpu();
		create_logical_device();
	}

	void Program::loop_program() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void Program::cleanup_program() {
		if (enable_validation_layers) {
			destroy_debug_utils_messenger_EXT(instance, debug_messenger, nullptr);
		}
		
		vkDestroyDevice(device, nullptr);

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