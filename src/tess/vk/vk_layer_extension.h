#ifndef TESS_VK_VK_LAYER_EXTENSION_H_
#define TESS_VK_VK_LAYER_EXTENSION_H_

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace tess
{
namespace vk
{
struct LayerProperties
{
  VkLayerProperties properties;
  std::vector<VkExtensionProperties> extensions;
};

class LayerExtension
{
public:
  LayerExtension();

  ~LayerExtension();

  VkResult GetInstanceLayerProperties();

  VkResult GetExtensionProperties(LayerProperties& layer_props);

  const auto& LayerPropertyList() const { return layer_property_list_; }

private:
  std::vector<LayerProperties> layer_property_list_;
  std::vector<std::string> app_requested_layer_names_;
  std::vector<std::string> app_requested_extension_names_;
};
}
}

#endif // TESS_VK_VK_LAYER_EXTENSION_H_
