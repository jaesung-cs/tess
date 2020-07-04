#ifndef TESS_VK_VK_LAYER_EXTENSION_H_
#define TESS_VK_VK_LAYER_EXTENSION_H_

#include <string>
#include <vector>
#include <unordered_set>

#include <vulkan/vulkan.h>

namespace tess
{
namespace vk
{
enum class Layer
{
  KHRONOS_VALIDATION,
};

enum class Extension
{
  KHR_SURFACE,
  EXT_DEBUG_UTILS,
};

/*
Previous implementations of Vulkan made a distinction between instance and device specific validation layers, 
but this is no longer the case. That means that the enabledLayerCount and ppEnabledLayerNames fields of 
VkDeviceCreateInfo are ignored by up-to-date implementations. However, it is still a good idea to set them 
anyway to be compatible with older implementations: ...

https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Logical_device_and_queues
*/
enum class DeviceExtension
{
  KHR_SWAPCHAIN,
};

struct LayerProperty
{
  VkLayerProperties property{};
  std::vector<VkExtensionProperties> extension_properties;
};

class LayerExtension
{
public:
  static const char* LayerName(Layer layer);
  static const char* ExtensionName(Extension extension);
  static const char* ExtensionName(DeviceExtension extension);

public:
  LayerExtension();

  ~LayerExtension();

  void LoadLayerExtensions();
  void PrintLayerExtensions();

  bool IsSupportedLayer(const std::string& layer_name);
  bool IsSupportedExtension(const std::string& extension_name);

private:
  void LoadExtensionProperties(LayerProperty& layer);

  std::vector<LayerProperty> layers_;
  std::vector<VkExtensionProperties> extensions_;

  std::unordered_set<std::string> layer_names_;
  std::unordered_set<std::string> extension_names_;
};
}
}

#endif // TESS_VK_VK_LAYER_EXTENSION_H_
