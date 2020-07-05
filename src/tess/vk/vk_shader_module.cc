#include "tess/vk/vk_shader_module.h"

#include <fstream>

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
//
// ShaderModuleCreator
//
ShaderModuleCreator::ShaderModuleCreator(Device device)
  : device_(device)
{
  create_info_.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info_.pNext = NULL;
  create_info_.flags = 0;
}

ShaderModuleCreator::~ShaderModuleCreator()
{
}

void ShaderModuleCreator::LoadCompiledSpirVFile(const std::string& filename)
{
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open())
    throw std::runtime_error("Failed to open file! filename: " + filename);

  size_t file_size = (size_t)file.tellg();
  code_.resize(file_size);
  file.seekg(0);
  file.read(code_.data(), file_size);
  file.close();

  create_info_.codeSize = code_.size();
  create_info_.pCode = reinterpret_cast<const uint32_t*>(code_.data());
}

ShaderModule ShaderModuleCreator::Create()
{
  ShaderModule shader_module{ device_ };

  if (VkResult result = vkCreateShaderModule(device_, &create_info_, NULL, &shader_module.shader_module_))
    throw Exception("Failed to create a shader module!", result);

  return shader_module;
}

//
// ShaderModule
//
ShaderModule::ShaderModule()
{
}

ShaderModule::ShaderModule(Device device)
  : device_(device)
{
}

ShaderModule::~ShaderModule()
{
}

void ShaderModule::Destroy()
{
  if (shader_module_)
  {
    vkDestroyShaderModule(device_, shader_module_, NULL);
    shader_module_ = NULL;
  }
}
}
}
