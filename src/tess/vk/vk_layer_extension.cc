#include "tess/vk/vk_layer_extension.h"

#include <iostream>

namespace tess
{
namespace vk
{
LayerExtension::LayerExtension()
{
}

LayerExtension::~LayerExtension()
{
}

VkResult LayerExtension::GetInstanceLayerProperties()
{
  uint32_t instance_layer_count;
  std::vector<VkLayerProperties> layer_properties;
  std::vector<LayerProperties> layer_property_list;
  VkResult result;

  do
  {
    result = vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
    if (result)
      return result;
    if (instance_layer_count == 0)
      return VK_INCOMPLETE;

    layer_properties.resize(instance_layer_count);
    result = vkEnumerateInstanceLayerProperties(&instance_layer_count, layer_properties.data());
  } while (result == VK_INCOMPLETE);

  std::cout << "Instance Layers" << std::endl
    << "================" << std::endl;

  for (auto global_layer_property : layer_properties)
  {
    std::cout << std::endl
      << global_layer_property.description << std::endl
      << "\t|---[Layer Name]--> " << global_layer_property.layerName << std::endl;

    LayerProperties layer_props;
    layer_props.properties = global_layer_property;

    // Load instance-level extensions
    result = GetExtensionProperties(layer_props);
    if (result)
      continue;

    layer_property_list.push_back(layer_props);

    for (auto j : layer_props.extensions)
      std::cout << "\t\t|---[Layer Extension]--> " << j.extensionName << std::endl;
  }

  /*
  Instanced Layers
  ================

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

VkResult LayerExtension::GetExtensionProperties(LayerProperties& layer_props)
{
  uint32_t extension_count;
  VkResult result;

  auto layer_name = layer_props.properties.layerName;
  do
  {
    result = vkEnumerateInstanceExtensionProperties(layer_name, &extension_count, NULL);

    if (result || extension_count == 0)
      continue;

    layer_props.extensions.resize(extension_count);
    result = vkEnumerateInstanceExtensionProperties(layer_name, &extension_count, layer_props.extensions.data());
  } while (result == VK_INCOMPLETE);

  return result;
}
}
}
