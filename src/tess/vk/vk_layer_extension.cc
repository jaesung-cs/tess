#include "tess/vk/vk_layer_extension.h"

#include <iostream>

namespace tess
{
namespace vk
{
const char* LayerExtension::LayerName(Layer layer)
{
  switch (layer)
  {
  case Layer::KHRONOS_VALIDATION: return "VK_LAYER_KHRONOS_validation";
  default: return nullptr;
  }
}

const char* LayerExtension::ExtensionName(Extension extension)
{
  switch (extension)
  {
  case Extension::KHR_SURFACE:     return VK_KHR_SURFACE_EXTENSION_NAME;
  case Extension::EXT_DEBUG_UTILS: return VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
  default: return nullptr;
  }
}

const char* LayerExtension::ExtensionName(DeviceExtension extension)
{
  switch (extension)
  {
  case DeviceExtension::KHR_SWAPCHAIN: return VK_KHR_SWAPCHAIN_EXTENSION_NAME;
  default: return nullptr;
  }
}


LayerExtension::LayerExtension()
{
}

LayerExtension::~LayerExtension()
{
}

void LayerExtension::LoadLayerExtensions()
{
  VkResult result;

  // Load layers
  uint32_t layer_count;
  result = vkEnumerateInstanceLayerProperties(&layer_count, NULL);
  std::vector<VkLayerProperties> layer_properties(layer_count);
  result = vkEnumerateInstanceLayerProperties(&layer_count, layer_properties.data());

  for (const auto& layer_property : layer_properties)
  {
    // Load layer extensions
    LayerProperty layer;
    layer.property = layer_property;
    LoadExtensionProperties(layer);

    layers_.push_back(std::move(layer));
  }

  // Load extensions
  uint32_t extension_count;
  result = vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
  extensions_.resize(extension_count);
  result = vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions_.data());

  // Register names
  for (const auto& layer : layers_)
    layer_names_.insert(layer.property.layerName);

  for (const auto& extension_property : extensions_)
    extension_names_.insert(extension_property.extensionName);
}

void LayerExtension::PrintLayerExtensions()
{
  std::cout << "Extensions" << std::endl
    << "==============" << std::endl;

  for (const auto& extension_property : extensions_)
    std::cout << "[Extension]--> " << extension_property.extensionName << std::endl;

  std::cout << std::endl
    << "Layers" << std::endl
    << "==============";
  for (const auto& layer : layers_)
  {
    std::cout << std::endl
      << layer.property.description << std::endl
      << "\t|---[Layer Name]--> " << layer.property.layerName << std::endl;

    for (const auto& extension_property : layer.extension_properties)
      std::cout << "\t\t|---[Layer Extension]--> " << extension_property.extensionName << std::endl;
  }

  /*
  Extensions
  ==============
  [Extension]--> VK_KHR_device_group_creation
  [Extension]--> VK_KHR_external_fence_capabilities
  [Extension]--> VK_KHR_external_memory_capabilities
  [Extension]--> VK_KHR_external_semaphore_capabilities
  [Extension]--> VK_KHR_get_physical_device_properties2
  [Extension]--> VK_KHR_get_surface_capabilities2
  [Extension]--> VK_KHR_surface
  [Extension]--> VK_KHR_surface_protected_capabilities
  [Extension]--> VK_KHR_win32_surface
  [Extension]--> VK_EXT_debug_report
  [Extension]--> VK_EXT_debug_utils
  [Extension]--> VK_EXT_swapchain_colorspace
  [Extension]--> VK_NV_external_memory_capabilities
  */

  /*
  Layers
  ==============
  NVIDIA Optimus layer
          |---[Layer Name]--> VK_LAYER_NV_optimus

  Open Broadcaster Software hook
          |---[Layer Name]--> VK_LAYER_OBS_HOOK

  NVIDIA Nsight Visual Studio Edition interception layer
          |---[Layer Name]--> VK_LAYER_NV_nsight
                  |---[Layer Extension]--> VK_EXT_debug_utils

  Steam Overlay Layer
          |---[Layer Name]--> VK_LAYER_VALVE_steam_overlay

  Steam Pipeline Caching Layer
          |---[Layer Name]--> VK_LAYER_VALVE_steam_fossilize

  LunarG API dump layer
          |---[Layer Name]--> VK_LAYER_LUNARG_api_dump

  LunarG device simulation layer
          |---[Layer Name]--> VK_LAYER_LUNARG_device_simulation

  GFXReconstruct Capture Layer Version 0.9.1
          |---[Layer Name]--> VK_LAYER_LUNARG_gfxreconstruct

  Khronos Validation Layer
          |---[Layer Name]--> VK_LAYER_KHRONOS_validation
                  |---[Layer Extension]--> VK_EXT_debug_report
                  |---[Layer Extension]--> VK_EXT_debug_utils
                  |---[Layer Extension]--> VK_EXT_validation_features

  Execution Monitoring Layer
          |---[Layer Name]--> VK_LAYER_LUNARG_monitor

  LunarG image capture layer
          |---[Layer Name]--> VK_LAYER_LUNARG_screenshot

  Vktrace tracing library
          |---[Layer Name]--> VK_LAYER_LUNARG_vktrace

  NVIDIA Nsight Systems profiler layer
          |---[Layer Name]--> VK_LAYER_NV_nsight-sys
                  |---[Layer Extension]--> VK_EXT_debug_report
                  |---[Layer Extension]--> VK_EXT_debug_utils

  LunarG Standard Validation Layer
          |---[Layer Name]--> VK_LAYER_LUNARG_standard_validation
  */
}

void LayerExtension::LoadExtensionProperties(LayerProperty& layer)
{
  uint32_t extension_count;
  VkResult result;

  const auto& layer_name = layer.property.layerName;
  result = vkEnumerateInstanceExtensionProperties(layer_name, &extension_count, NULL);
  layer.extension_properties.resize(extension_count);
  result = vkEnumerateInstanceExtensionProperties(layer_name, &extension_count, layer.extension_properties.data());
}

bool LayerExtension::IsSupportedLayer(const std::string& layer_name)
{
  return layer_names_.find(layer_name) != layer_names_.cend();
}

bool LayerExtension::IsSupportedExtension(const std::string& extension_name)
{
  return extension_names_.find(extension_name) != extension_names_.cend();
}
}
}
