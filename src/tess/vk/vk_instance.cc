#include "tess/vk/vk_instance.h"

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
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
    vkDestroyInstance(instance_, NULL);
    instance_ = nullptr;
  }
}
}
}
