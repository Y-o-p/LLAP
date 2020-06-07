#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <string>
#include <optional>

#include "debug.h"

namespace LLAP {

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphics_family;
	};

	class Program {
	protected:
		GLFWwindow* window;

	private:
		VkInstance instance;
		VkDebugUtilsMessengerEXT debug_messenger;

		const std::vector<const char*> validation_layers = {
			"VK_LAYER_KHRONOS_validation",
		};

		bool check_validation_support();
		std::vector<const char*> get_required_extensions();
		
		static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
			VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
			VkDebugUtilsMessageTypeFlagsEXT message_type,
			const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
			void* user_data);

		void populate_debug_messenger_create_info(
			VkDebugUtilsMessengerCreateInfoEXT& create_info);
		void setup_debug_messenger();
		VkResult create_debug_utils_messenger_EXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* create_info,
			const VkAllocationCallbacks* allocator,
			VkDebugUtilsMessengerEXT* debug_messenger);

		static void destroy_debug_utils_messenger_EXT(
			VkInstance instance,
			VkDebugUtilsMessengerEXT debug_messenger,
			const VkAllocationCallbacks* allocator);

#ifdef NDEBUG
		const bool enable_validation_layers = false;
#else
		const bool enable_validation_layers = true;
#endif

		// Device
		VkPhysicalDevice physical_device = VK_NULL_HANDLE;
		void pick_gpu();
		bool is_gpu_suitable(VkPhysicalDevice device);

		// Queue
		QueueFamilyIndices find_queue_families(VkPhysicalDevice device);

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