#ifndef TESS_VK_VK_LAYER_EXTENSION_H_
#define TESS_VK_VK_LAYER_EXTENSION_H_

#include <string>
#include <vector>

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
  KHR_SWAPCHAIN,
  KHR_SURFACE,
  EXT_DEBUG_UTILS,
};

class LayerExtension
{
private:
  static const char* LayerName(Layer layer);
  static const char* ExtensionName(Extension extension);

public:
  LayerExtension();

  ~LayerExtension();

  void LoadLayers();

private:
  struct LayerProperty
  {
    VkLayerProperties property{};
    std::vector<VkExtensionProperties> extension_properties;
  };

  void LoadExtensionProperties(LayerProperty& layer);

  std::vector<LayerProperty> layers_;
};
}
}

#endif // TESS_VK_VK_LAYER_EXTENSION_H_
