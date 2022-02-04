#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <string>
#include <optional>
#include <set>
#include <algorithm>

#include "debug.h"
#include "io.h"

namespace LLAP {

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	class Program {
	protected:
		GLFWwindow* window;
		static const int WIDTH = 800, HEIGHT = 600;
		static const int MAX_FRAMES_IN_FLIGHT = 2;

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

		const std::vector<const char*> device_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		// Swap chain
		std::vector<VkFramebuffer> swap_chain_framebuffers;
		VkSwapchainKHR swap_chain;
		std::vector<VkImage> swap_chain_images;
		std::vector<VkImageView> swap_chain_image_views;
		VkFormat swap_chain_image_format;
		VkExtent2D swap_chain_extent;
		void create_frame_buffers();
		SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);
		VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);
		VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes);
		VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);
		void create_swap_chain();
		void create_image_views();

		bool check_device_extension_support(VkPhysicalDevice device);

#ifdef NDEBUG
		const bool enable_validation_layers = false;
#else
		const bool enable_validation_layers = true;
#endif

		// Surface
		VkSurfaceKHR surface;
		void create_surface();

		// Device
		VkPhysicalDevice physical_device = VK_NULL_HANDLE;
		void pick_gpu();
		bool is_gpu_suitable(VkPhysicalDevice device);

		// Logical device
		VkDevice device;
		void create_logical_device();

		// Queue
		VkQueue graphics_queue;
		VkQueue present_queue;
		QueueFamilyIndices find_queue_families(VkPhysicalDevice device);

		// Command buffers
		VkCommandPool command_pool;
		std::vector<VkCommandBuffer> command_buffers;
		void create_command_pool();
		void create_command_buffers();

		// Graphics pipeline
		VkPipeline graphics_pipeline;
		VkPipelineLayout pipeline_layout;
		void create_graphics_pipeline();
		VkShaderModule create_shader_module(const std::vector<char>& code);

		// Render pass
		VkRenderPass render_pass;
		void create_render_pass();
		void draw_frame();

		// Semaphores
		VkSemaphore image_available_s;
		VkSemaphore render_finished_s;
		void create_semaphores();

		size_t current_frame = 0;
		std::vector<VkFence> in_flight_fences;
		std::vector<VkFence> in_flight_images;
		std::vector<VkSemaphore> image_available_semaphores;
		std::vector<VkSemaphore> render_finished_semaphores;

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