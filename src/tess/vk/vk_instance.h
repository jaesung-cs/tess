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
  explicit InstanceCreator(const std::string& app_name = "");

  ~InstanceCreator();

  void SetLayerExtension(const std::vector<const char*>& layers, const std::vector<const char*>& extensions);

  Instance Create();

private:
  VkApplicationInfo app_info_{};
  VkInstanceCreateInfo instance_info_{};
};

class Instance
{
  friend class InstanceCreator;

public:
  Instance();

  ~Instance();

  void Destroy();

  operator VkInstance ()
  {
    return instance_;
  }

private:
  VkInstance instance_ = nullptr;

#ifdef NDEBUG
  bool enable_validation_layers_ = false;
#else
  bool enable_validation_layers_ = true;
#endif

  VkDebugUtilsMessengerEXT debug_messenger_ = nullptr;
};
}
}

#endif // TESS_VK_VK_INSTANCE_H_
