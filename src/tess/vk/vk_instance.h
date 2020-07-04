#ifndef TESS_VK_VK_INSTANCE_H_
#define TESS_VK_VK_INSTANCE_H_

#include <string>
#include <vector>
#include <optional>

#include <vulkan/vulkan.h>

#include "tess/vk/vk_layer_extension.h"

namespace tess
{
namespace vk
{
class Instance;

class InstanceCreator
{
public:
  InstanceCreator(const std::string& app_name = "");

  ~InstanceCreator();

  void EnableValidationLayer();

  void AddLayer(Layer layer);
  void AddLayer(const std::string& layer_name);
  void AddExtension(Extension extension);
  void AddExtension(const std::string& extension_name);

  Instance Create();

private:
  std::vector<std::string> layers_;
  std::vector<std::string> extensions_;

  VkApplicationInfo app_info_{};
  VkInstanceCreateInfo instance_info_{};

  bool enable_validation_layers_ = false;
  vk::LayerExtension layer_extension_;
};

class Instance
{
  friend class InstanceCreator;

public:
  Instance();

  ~Instance();

  void Destroy();

  operator VkInstance () { return instance_; }

private:
  VkInstance instance_ = nullptr;
};
}
}

#endif // TESS_VK_VK_INSTANCE_H_
