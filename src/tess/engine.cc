#include "tess/engine.h"

#include <iostream>
#include <fstream>
#include <set>
#include <cstdint>
#include <algorithm>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace tess
{
// GLFW Callback functions
namespace
{
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  auto engine = (Engine*)glfwGetWindowUserPointer(window);
  engine->Resize(width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  double x, y;
  glfwGetCursorPos(window, &x, &y);

  auto engine = (Engine*)glfwGetWindowUserPointer(window);
  engine->MouseButton(button, action, mods, x, y);
}

void cursor_pos_callback(GLFWwindow* window, double x, double y)
{
  auto engine = (Engine*)glfwGetWindowUserPointer(window);
  engine->MouseMove(x, y);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  auto engine = (Engine*)glfwGetWindowUserPointer(window);
  engine->Keyboard(key, action, mods);
}
}

// Vulkan callback functions
namespace
{
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
  VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
  VkDebugUtilsMessageTypeFlagsEXT message_type,
  const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
  void* pUserData)
{
  /*
  message_severity:
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: Diagnostic message
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: Informational message like the creation of a resource
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: Message about behavior that is not necessarily an error, but very likely a bug in your application
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: Message about behavior that is invalid and may cause crashes

  message_type:
    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: Some event has happened that is unrelated to the specification or performance
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: Something has happened that violates the specification or indicates a possible mistake
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: Potential non-optimal use of Vulkan

  callback_data:
    ->pMessage: The debug message as a null-terminated string
    ->pObjects: Array of Vulkan object handles related to the message
    ->objectCount: Number of objects in array
  */

  std::cerr << "validation layer: " << callback_data->pMessage << std::endl;

  return VK_FALSE;
}
}

// Vulkan proxy functions
namespace
{
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr)
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  else
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr)
    func(instance, debugMessenger, pAllocator);
}
}

// Utility functions
namespace
{
std::vector<char> ReadFile(const std::string& filename)
{
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open())
    throw std::runtime_error("failed to open file!");

  size_t file_size = (size_t)file.tellg();
  std::vector<char> buffer(file_size);

  file.seekg(0);
  file.read(buffer.data(), file_size);

  file.close();

  return buffer;
}
}

Engine::Engine() = default;

Engine::Engine(int width, int height)
  : width_(width), height_(height)
{
}

Engine::~Engine()
{
  glfwDestroyWindow(window_);
}

void Engine::Resize(int width, int height)
{
  framebuffer_resized_ = true;
}

void Engine::MouseButton(int button, int action, int mods, double x, double y)
{
}

void Engine::MouseMove(double x, double y)
{
}

void Engine::Keyboard(int key, int action, int mods)
{
}

void Engine::Run()
{
  Initialize();

  CheckVulkanExtensionSupport();

  while (!glfwWindowShouldClose(window_))
  {
    // Update events
    glfwPollEvents();

    DrawFrame();
  }

  vkDeviceWaitIdle(device_);

  Cleanup();
}

void Engine::Initialize()
{
  InitializeGlfw();
  InitializeVulkan();
}

void Engine::InitializeGlfw()
{
  // GLFW init
  glfwInit();

  if (glfwVulkanSupported())
    std::cout << "Vulkan is supported" << std::endl;
  else
  {
    std::cout << "Vulkan is not supported" << std::endl;
    return;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // GLFW create window
  window_ = glfwCreateWindow(width_, height_, "tess", NULL, NULL);
  if (window_ == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return;
  }

  glfwMakeContextCurrent(window_);

  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
  glfwSetKeyCallback(window_, key_callback);
  glfwSetMouseButtonCallback(window_, mouse_button_callback);
  glfwSetCursorPosCallback(window_, cursor_pos_callback);
}

void Engine::InitializeVulkan()
{
  CreateInstance();
  SetupDebugMessenger();
  CreateSurface();
  PickPhysicalDevice();
  CreateLogicalDevice();
  CreateSwapChain();
  CreateImageViews();
  CreateRenderPass();
  CreateGraphicsPipeline();
  CreateFramebuffers();
  CreateCommandPool();
  CreateCommandBuffers();
  CreateSyncObjects();
}

void Engine::CleanupSwapChain()
{
  for (auto framebuffer : swap_chain_framebuffers_)
    vkDestroyFramebuffer(device_, framebuffer, nullptr);

  vkFreeCommandBuffers(device_, command_pool_, static_cast<uint32_t>(command_buffers_.size()), command_buffers_.data());

  vkDestroyPipeline(device_, graphics_pipeline_, nullptr);
  vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
  vkDestroyRenderPass(device_, render_pass_, nullptr);

  for (auto image_view : swap_chain_image_views_)
    vkDestroyImageView(device_, image_view, nullptr);

  vkDestroySwapchainKHR(device_, swap_chain_, nullptr);
}

void Engine::RecreateSwapChain()
{
  glfwGetFramebufferSize(window_, &width_, &height_);
  while (width_ == 0 || height_ == 0)
  {
    glfwGetFramebufferSize(window_, &width_, &height_);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(device_);

  CleanupSwapChain();

  CreateSwapChain();
  CreateImageViews();
  CreateRenderPass();
  CreateGraphicsPipeline();
  CreateFramebuffers();
  CreateCommandBuffers();
}

void Engine::CreateInstance()
{
  if (enable_validation_layers_ && !CheckValidationLayerSupport())
    throw std::runtime_error("validation layers requested, but not available!");

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Tess";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  auto extensions = GetRequiredExtensions();
  create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  create_info.ppEnabledExtensionNames = extensions.data();

  if (enable_validation_layers_)
  {
    create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers_.size());
    create_info.ppEnabledLayerNames = validation_layers_.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    PopulateDebugMessengerCreateInfo(debug_create_info);
    create_info.pNext = &debug_create_info;
  }
  else
  {
    create_info.enabledLayerCount = 0;

    create_info.pNext = nullptr;
  }

  if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS)
    throw std::runtime_error("failed to create Vulkan instance!");
}

void Engine::Cleanup()
{
  CleanupSwapChain();

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
  {
    vkDestroySemaphore(device_, render_finished_semaphores_[i], nullptr);
    vkDestroySemaphore(device_, image_available_semaphores_[i], nullptr);
    vkDestroyFence(device_, in_flight_fences_[i], nullptr);
  }

  vkDestroyCommandPool(device_, command_pool_, nullptr);

  vkDestroyShaderModule(device_, frag_shader_module_, nullptr);
  vkDestroyShaderModule(device_, vert_shader_module_, nullptr);

  vkDestroyDevice(device_, nullptr);

  if (enable_validation_layers_)
    DestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);

  vkDestroySurfaceKHR(instance_, surface_, nullptr);
  vkDestroyInstance(instance_, nullptr);

  glfwDestroyWindow(window_);
  glfwTerminate();
}

void Engine::CheckVulkanExtensionSupport()
{
  uint32_t extension_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

  std::vector<VkExtensionProperties> extensions(extension_count);

  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

  std::cout << "available extensions:\n";
  for (const auto& extension : extensions)
    std::cout << '\t' << extension.extensionName << '\n';

  /*
  available extensions:
        VK_KHR_device_group_creation
        VK_KHR_external_fence_capabilities
        VK_KHR_external_memory_capabilities
        VK_KHR_external_semaphore_capabilities
        VK_KHR_get_physical_device_properties2
        VK_KHR_get_surface_capabilities2
        VK_KHR_surface
        VK_KHR_surface_protected_capabilities
        VK_KHR_win32_surface
        VK_EXT_debug_report
        VK_EXT_debug_utils
        VK_EXT_swapchain_colorspace
        VK_NV_external_memory_capabilities
  */
}

bool Engine::CheckValidationLayerSupport()
{
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (const char* layer_name : validation_layers_)
  {
    bool layer_found = false;

    for (const auto& layer_properties : available_layers)
    {
      if (strcmp(layer_name, layer_properties.layerName) == 0)
      {
        layer_found = true;
        break;
      }
    }

    if (!layer_found)
      return false;
  }

  return true;
}

std::vector<const char*> Engine::GetRequiredExtensions()
{
  uint32_t glfw_extension_count = 0;
  const char** glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

  if (enable_validation_layers_)
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  return extensions;
}

void Engine::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info)
{
  create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = debug_callback;
  create_info.pUserData = this; // Optional
}

void Engine::SetupDebugMessenger()
{
  if (!enable_validation_layers_) return;

  VkDebugUtilsMessengerCreateInfoEXT create_info{};
  PopulateDebugMessengerCreateInfo(create_info);

  if (CreateDebugUtilsMessengerEXT(instance_, &create_info, nullptr, &debug_messenger_) != VK_SUCCESS)
    throw std::runtime_error("failed to set up debug messenger!");
}

void Engine::PickPhysicalDevice()
{
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);

  if (device_count == 0)
    throw std::runtime_error("failed to find GPUs with Vulkan support!");

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

  for (const auto& device : devices)
  {
    if (IsDeviceSuitable(device))
    {
      physical_device_ = device;
      break;
    }
  }

  if (physical_device_ == VK_NULL_HANDLE)
    throw std::runtime_error("failed to find a suitable GPU!");
}

void Engine::CreateLogicalDevice()
{
  QueueFamilyIndices indices = FindQueueFamilies(physical_device_);

  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

  // Remove duplicates of queue families
  std::set<uint32_t> unique_queue_families = { indices.graphics_family.value(), indices.present_family.value() };

  float queue_priority = 1.0f;
  for (uint32_t queue_family : unique_queue_families)
  {
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.emplace_back(queue_create_info);
  }

  VkPhysicalDeviceFeatures device_features{};

  VkDeviceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
  create_info.pQueueCreateInfos = queue_create_infos.data();

  create_info.pEnabledFeatures = &device_features;

  create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions_.size());
  create_info.ppEnabledExtensionNames = device_extensions_.data();

  if (enable_validation_layers_)
  {
    create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers_.size());
    create_info.ppEnabledLayerNames = validation_layers_.data();
  }
  else
    create_info.enabledLayerCount = 0;

  if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_) != VK_SUCCESS)
    throw std::runtime_error("failed to create logical device!");

  vkGetDeviceQueue(device_, indices.graphics_family.value(), 0, &graphics_queue_);
  vkGetDeviceQueue(device_, indices.present_family.value(), 0, &present_queue_);
}

void Engine::CreateSurface()
{
  if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) != VK_SUCCESS)
    throw std::runtime_error("failed to create window surface!");
}

Engine::QueueFamilyIndices Engine::FindQueueFamilies(VkPhysicalDevice device)
{
  QueueFamilyIndices indices;

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

  int i = 0;
  for (const auto& queue_family : queue_families)
  {
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &present_support);

    if (present_support)
      indices.present_family = i;

    if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      indices.graphics_family = i;

    if (indices.IsComplete())
      break;

    i++;
  }

  return indices;
}

int Engine::IsDeviceSuitable(VkPhysicalDevice device)
{
  QueueFamilyIndices indices = FindQueueFamilies(device);

  bool extensions_supported = CheckDeviceExtensionSupport(device);

  bool swap_chain_adequate = false;
  if (extensions_supported) {
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
    swap_chain_adequate = !swapChainSupport.formats.empty() && !swapChainSupport.present_modes.empty();
  }

  return indices.IsComplete() && extensions_supported && swap_chain_adequate;
}

bool Engine::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

  std::cout << "Device available extensions:\n";
  for (const auto& extension : available_extensions)
    std::cout << '\t' << extension.extensionName << '\n';

  std::set<std::string> required_extensions(device_extensions_.begin(), device_extensions_.end());

  for (const auto& extension : available_extensions)
    required_extensions.erase(extension.extensionName);

  return required_extensions.empty();
}

Engine::SwapChainSupportDetails Engine::QuerySwapChainSupport(VkPhysicalDevice device)
{
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, nullptr);

  if (format_count != 0)
  {
    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, details.formats.data());
  }

  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, nullptr);

  if (present_mode_count != 0) {
    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, details.present_modes.data());
  }

  return details;
}

VkExtent2D Engine::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
  if (capabilities.currentExtent.width != UINT32_MAX)
    return capabilities.currentExtent;
  else
  {
    glfwGetFramebufferSize(window_, &width_, &height_);

    VkExtent2D actual_extent = { width_, height_ };

    actual_extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
    actual_extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actual_extent.height));

    return actual_extent;
  }
}

VkSurfaceFormatKHR Engine::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats)
{
  for (const auto& available_format : available_formats)
  {
    if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      return available_format;
  }

  // Choose the first one if it fails to find BGRA8 SRGB format
  return available_formats[0];
}

VkPresentModeKHR Engine::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes)
{
  /*
  VK_PRESENT_MODE_IMMEDIATE_KHR: Images submitted by your application are transferred to the screen right away, which may result in tearing.
  VK_PRESENT_MODE_FIFO_KHR: The swap chain is a queue where the display takes an image from the front of the queue when the display is refreshed and the program inserts rendered images at the back of the queue. If the queue is full then the program has to wait. This is most similar to vertical sync as found in modern games. The moment that the display is refreshed is known as "vertical blank".
  VK_PRESENT_MODE_FIFO_RELAXED_KHR: This mode only differs from the previous one if the application is late and the queue was empty at the last vertical blank. Instead of waiting for the next vertical blank, the image is transferred right away when it finally arrives. This may result in visible tearing.
  VK_PRESENT_MODE_MAILBOX_KHR: This is another variation of the second mode. Instead of blocking the application when the queue is full, the images that are already queued are simply replaced with the newer ones. This mode can be used to implement triple buffering, which allows you to avoid tearing with significantly less latency issues than standard vertical sync that uses double buffering.
  */

  for (const auto& available_present_mode : available_present_modes)
  {
    if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
      return available_present_mode;
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

void Engine::CreateSwapChain()
{
  SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(physical_device_);

  VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats);
  VkPresentModeKHR present_mode = ChooseSwapPresentMode(swap_chain_support.present_modes);
  VkExtent2D extent = ChooseSwapExtent(swap_chain_support.capabilities);

  uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;

  if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount)
    image_count = swap_chain_support.capabilities.maxImageCount;

  VkSwapchainCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = surface_;

  create_info.minImageCount = image_count;
  create_info.imageFormat = surface_format.format;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = extent;
  create_info.imageArrayLayers = 1; // 1 unless 3D stereoscopic
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices = FindQueueFamilies(physical_device_);
  uint32_t queueFamilyIndices[] = { indices.graphics_family.value(), indices.present_family.value() };

  if (indices.graphics_family != indices.present_family)
  {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queueFamilyIndices;
  }
  else
  {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0; // optional
    create_info.pQueueFamilyIndices = nullptr; // optional
  }

  create_info.preTransform = swap_chain_support.capabilities.currentTransform;

  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;

  create_info.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device_, &create_info, nullptr, &swap_chain_) != VK_SUCCESS)
    throw std::runtime_error("failed to create swap chain!");

  vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count, nullptr);
  swap_chain_images_.resize(image_count);
  vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count, swap_chain_images_.data());

  swap_chain_image_format_ = surface_format.format;
  swap_chain_extent_ = extent;
}

void Engine::CreateImageViews()
{
  swap_chain_image_views_.resize(swap_chain_images_.size());

  for (size_t i = 0; i < swap_chain_images_.size(); i++)
  {
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = swap_chain_images_[i];

    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = swap_chain_image_format_;

    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    // Could create multiple image views for each image representing the views for the left and right eyes by accessing different layers.
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device_, &create_info, nullptr, &swap_chain_image_views_[i]) != VK_SUCCESS)
      throw std::runtime_error("failed to create image views!");
  }
}

void Engine::CreateGraphicsPipeline()
{
  auto vert_shader_code = ReadFile("../src/shader/vert.spv");
  auto frag_shader_code = ReadFile("../src/shader/frag.spv");

  vert_shader_module_ = CreateShaderModule(vert_shader_code);
  frag_shader_module_ = CreateShaderModule(frag_shader_code);

  VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
  vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_shader_stage_info.module = vert_shader_module_;
  vert_shader_stage_info.pName = "main";

  VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
  frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_shader_stage_info.module = frag_shader_module_;
  frag_shader_stage_info.pName = "main";

  VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };

  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = 0;
  vertex_input_info.pVertexBindingDescriptions = nullptr; // optional
  vertex_input_info.vertexAttributeDescriptionCount = 0;
  vertex_input_info.pVertexAttributeDescriptions = nullptr; // optional

  VkPipelineInputAssemblyStateCreateInfo input_assembly{};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)swap_chain_extent_.width;
  viewport.height = (float)swap_chain_extent_.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = { 0, 0 };
  scissor.extent = swap_chain_extent_;

  VkPipelineViewportStateCreateInfo viewport_state{};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;

  rasterizer.rasterizerDiscardEnable = VK_FALSE;

  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;

  rasterizer.lineWidth = 1.0f;

  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f; // Optional
  rasterizer.depthBiasClamp = 0.0f; // Optional
  rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f; // Optional
  multisampling.pSampleMask = nullptr; // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable = VK_FALSE; // Optional

  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

  VkPipelineColorBlendStateCreateInfo color_blending{};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;
  color_blending.blendConstants[0] = 0.0f; // Optional
  color_blending.blendConstants[1] = 0.0f; // Optional
  color_blending.blendConstants[2] = 0.0f; // Optional
  color_blending.blendConstants[3] = 0.0f; // Optional

  VkDynamicState dynamic_states[] =
  {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_LINE_WIDTH
  };

  VkPipelineDynamicStateCreateInfo dynamic_state{};
  dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state.dynamicStateCount = 2;
  dynamic_state.pDynamicStates = dynamic_states;

  VkPipelineLayoutCreateInfo pipeline_layout_info{};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 0; // Optional
  pipeline_layout_info.pSetLayouts = nullptr; // Optional
  pipeline_layout_info.pushConstantRangeCount = 0; // Optional
  pipeline_layout_info.pPushConstantRanges = nullptr; // Optional

  if (vkCreatePipelineLayout(device_, &pipeline_layout_info, nullptr, &pipeline_layout_) != VK_SUCCESS)
    throw std::runtime_error("failed to create pipeline layout!");

  VkGraphicsPipelineCreateInfo pipeline_info{};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = 2;
  pipeline_info.pStages = shader_stages;

  pipeline_info.pVertexInputState = &vertex_input_info;
  pipeline_info.pInputAssemblyState = &input_assembly;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterizer;
  pipeline_info.pMultisampleState = &multisampling;
  pipeline_info.pDepthStencilState = nullptr; // Optional
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.pDynamicState = nullptr; // Optional

  pipeline_info.layout = pipeline_layout_;

  pipeline_info.renderPass = render_pass_;
  pipeline_info.subpass = 0;

  pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
  pipeline_info.basePipelineIndex = -1; // Optional

  if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline_) != VK_SUCCESS)
    throw std::runtime_error("failed to create graphics pipeline!");
}

VkShaderModule Engine::CreateShaderModule(const std::vector<char>& code)
{
  VkShaderModuleCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

  VkShaderModule shader_module;
  if (vkCreateShaderModule(device_, &create_info, nullptr, &shader_module) != VK_SUCCESS)
    throw std::runtime_error("failed to create shader module!");

  return shader_module;
}

void Engine::CreateRenderPass()
{
  VkAttachmentDescription color_attachment{};
  color_attachment.format = swap_chain_image_format_;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_ref{};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;

  VkRenderPassCreateInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 1;
  render_pass_info.pAttachments = &color_attachment;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;

  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;

  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(device_, &render_pass_info, nullptr, &render_pass_) != VK_SUCCESS)
    throw std::runtime_error("failed to create render pass!");
}

void Engine::CreateFramebuffers()
{
  swap_chain_framebuffers_.resize(swap_chain_image_views_.size());

  for (size_t i = 0; i < swap_chain_image_views_.size(); i++)
  {
    VkImageView attachments[] =
    {
      swap_chain_image_views_[i]
    };

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = render_pass_;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = swap_chain_extent_.width;
    framebufferInfo.height = swap_chain_extent_.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device_, &framebufferInfo, nullptr, &swap_chain_framebuffers_[i]) != VK_SUCCESS)
      throw std::runtime_error("failed to create framebuffer!");
  }
}

void Engine::CreateCommandPool()
{
  QueueFamilyIndices queue_family_indices = FindQueueFamilies(physical_device_);

  VkCommandPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
  pool_info.flags = 0; // Optional

  if (vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool_) != VK_SUCCESS)
    throw std::runtime_error("failed to create command pool!");
}

void Engine::CreateCommandBuffers()
{
  command_buffers_.resize(swap_chain_framebuffers_.size());

  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = command_pool_;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = (uint32_t)command_buffers_.size();

  if (vkAllocateCommandBuffers(device_, &alloc_info, command_buffers_.data()) != VK_SUCCESS)
    throw std::runtime_error("failed to allocate command buffers!");

  for (size_t i = 0; i < command_buffers_.size(); i++)
  {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0; // Optional
    begin_info.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(command_buffers_[i], &begin_info) != VK_SUCCESS)
      throw std::runtime_error("failed to begin recording command buffer!");

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = render_pass_;
    render_pass_info.framebuffer = swap_chain_framebuffers_[i];

    render_pass_info.renderArea.offset = { 0, 0 };
    render_pass_info.renderArea.extent = swap_chain_extent_;

    VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clearColor;

    vkCmdBeginRenderPass(command_buffers_[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);

    vkCmdDraw(command_buffers_[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(command_buffers_[i]);

    if (vkEndCommandBuffer(command_buffers_[i]) != VK_SUCCESS)
      throw std::runtime_error("failed to record command buffer!");
  }
}

void Engine::DrawFrame()
{
  vkWaitForFences(device_, 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX);

  uint32_t image_index;
  VkResult result = vkAcquireNextImageKHR(device_, swap_chain_, UINT64_MAX, image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &image_index);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebuffer_resized_)
  {
    framebuffer_resized_ = false;
    RecreateSwapChain();
    return;
  }
  else if (result != VK_SUCCESS)
    throw std::runtime_error("failed to acquire swap chain image!");

  // Check if a previous frame is using this image (i.e. there is its fence to wait on)
  if (images_in_flight_[image_index] != VK_NULL_HANDLE)
    vkWaitForFences(device_, 1, &images_in_flight_[image_index], VK_TRUE, UINT64_MAX);
  // Mark the image as now being in use by this frame
  images_in_flight_[image_index] = in_flight_fences_[current_frame_];

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore wait_semaphores[] = { image_available_semaphores_[current_frame_] };
  VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_stages;

  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffers_[image_index];

  VkSemaphore signal_semaphores[] = { render_finished_semaphores_[current_frame_] };
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_semaphores;

  vkResetFences(device_, 1, &in_flight_fences_[current_frame_]);

  if (vkQueueSubmit(graphics_queue_, 1, &submit_info, in_flight_fences_[current_frame_]) != VK_SUCCESS)
    throw std::runtime_error("failed to submit draw command buffer!");

  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signal_semaphores;

  VkSwapchainKHR swap_chains[] = { swap_chain_ };
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swap_chains;
  present_info.pImageIndices = &image_index;

  present_info.pResults = nullptr; // Optional

  result = vkQueuePresentKHR(present_queue_, &present_info);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebuffer_resized_)
  {
    framebuffer_resized_ = false;
    RecreateSwapChain();
  }
  else if (result != VK_SUCCESS)
    throw std::runtime_error("failed to present swap chain image!");

  vkQueueWaitIdle(present_queue_);

  current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Engine::CreateSyncObjects()
{
  image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
  render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
  in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);
  images_in_flight_.resize(swap_chain_images_.size(), VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_info{};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
  {
    if (vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &image_available_semaphores_[i]) != VK_SUCCESS ||
      vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &render_finished_semaphores_[i]) != VK_SUCCESS ||
      vkCreateFence(device_, &fence_info, nullptr, &in_flight_fences_[i]) != VK_SUCCESS)
      throw std::runtime_error("failed to create semaphores for a frame!");
  }
}
}
