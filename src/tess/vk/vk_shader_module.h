#ifndef TESS_VK_VK_SHADER_MODULE_H_
#define TESS_VK_VK_SHADER_MODULE_H_

#include <vulkan/vulkan.h>

#include "tess/vk/vk_device.h"

namespace tess
{
namespace vk
{
class ShaderModule;

class ShaderModuleCreator
{
public:
  ShaderModuleCreator() = delete;

  explicit ShaderModuleCreator(Device device);

  ~ShaderModuleCreator();

  void LoadCompiledSpirVFile(const std::string& filename);

  ShaderModule Create();

private:
  VkShaderModuleCreateInfo create_info_{};

  std::vector<char> code_;

  Device device_;
};

class ShaderModule
{
  friend class ShaderModuleCreator;

public:
  ShaderModule();

  explicit ShaderModule(Device device);

  ~ShaderModule();

  void Destroy();

  operator VkShaderModule () { return shader_module_; }

private:
  Device device_;

  VkShaderModule shader_module_ = NULL;
};
}
}

#endif // TESS_VK_VK_SHADER_MODULE_H_
