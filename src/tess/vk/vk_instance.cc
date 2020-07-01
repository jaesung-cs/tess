#include "tess/vk/vk_instance.h"

#include <iostream>

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
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


InstanceCreator::InstanceCreator(const std::string& app_name)
{
  app_info_.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info_.pNext = NULL;
  app_info_.pApplicationName = app_name.c_str();
  app_info_.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info_.pEngineName = "No Engine";
  app_info_.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info_.apiVersion = VK_API_VERSION_1_0;

  instance_info_.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info_.pNext = NULL;
  instance_info_.flags = 0;
  instance_info_.pApplicationInfo = &app_info_;

  instance_info_.enabledLayerCount = 0;
  instance_info_.ppEnabledLayerNames = NULL;
  instance_info_.enabledExtensionCount = 0;
  instance_info_.ppEnabledExtensionNames = NULL;

  /*
  Layer:
  VK_LAYER_LUNARG_api_dump
  VK_LAYER_KHRONOS_validation

  Extensions:
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
  VK_KHR_SURFACE_EXTENSION_NAME
  VK_KHR_win32_surface
  VK_EXT_DEBUG_UTILS_EXTENSION_NAME
  */
}

InstanceCreator::~InstanceCreator()
{
}

void InstanceCreator::SetLayerExtension(const std::vector<const char*>& layers, const std::vector<const char*>& extensions)
{
  instance_info_.enabledLayerCount = layers.size();
  instance_info_.ppEnabledLayerNames = layers.data();

  instance_info_.enabledExtensionCount = extensions.size();
  instance_info_.ppEnabledExtensionNames = extensions.data();
}

Instance InstanceCreator::Create()
{
  Instance instance;

  auto result = vkCreateInstance(&instance_info_, NULL, &instance.instance_);
  if (result != VK_SUCCESS)
    throw Exception("Error creating instance", result);

  // Setup debug messenger
  if (instance.enable_validation_layers_)
  {
    VkDebugUtilsMessengerCreateInfoEXT create_info{};
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debug_callback;
    create_info.pUserData = instance; // Optional

    if (CreateDebugUtilsMessengerEXT(instance, &create_info, nullptr, &instance.debug_messenger_) != VK_SUCCESS)
      throw std::runtime_error("failed to set up debug messenger!");
  }

  return instance;
}

Instance::Instance()
{
}

Instance::~Instance()
{
}

void Instance::Destroy()
{
  if (instance_)
  {
    if (enable_validation_layers_)
      DestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);

    vkDestroyInstance(instance_, NULL);
    instance_ = nullptr;
  }
}
}
}
