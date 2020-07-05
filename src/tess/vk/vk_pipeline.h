#ifndef TESS_VK_VK_PIPELINE_H_
#define TESS_VK_VK_PIPELINE_H_

#include <vulkan/vulkan.h>

#include "tess/vk/vk_device.h"
#include "tess/vk/vk_pipeline_layout.h"
#include "tess/vk/vk_render_pass.h"
#include "tess/vk/vk_shader_module.h"

namespace tess
{
namespace vk
{
class Pipeline;

class GraphicsPipelineCreator
{
public:
  GraphicsPipelineCreator() = delete;

  explicit GraphicsPipelineCreator(Device device);

  ~GraphicsPipelineCreator();

  void AddVertexShaderStage(ShaderModule shader_module);
  void AddFragmentShaderStage(ShaderModule shader_module);

  void SetAttributeStride(int stride);
  void SetAttribute(int location, int size, int offset);

  void SetViewport(float width, float height);
  void SetViewport(float x, float y, float width, float height);
  void SetPipelineLayout(PipelineLayout pipeline_layout);
  void SetRenderPass(RenderPass render_pass);

  Pipeline Create();

private:
  std::vector<VkPipelineShaderStageCreateInfo> shader_stages_;

  VkVertexInputBindingDescription binding_description_{};
  std::vector<VkVertexInputAttributeDescription> attribute_descriptions_{};
  VkPipelineVertexInputStateCreateInfo vertex_input_info_{};

  VkPipelineInputAssemblyStateCreateInfo input_assembly_{};

  VkViewport viewport_{};
  VkRect2D scissor_{};
  VkPipelineViewportStateCreateInfo viewport_state_{};

  VkPipelineRasterizationStateCreateInfo rasterizer_{};

  VkPipelineMultisampleStateCreateInfo multisampling_{};

  VkPipelineColorBlendAttachmentState color_blend_attachment_{};
  VkPipelineColorBlendStateCreateInfo color_blending_{};

  std::vector<VkDynamicState> dynamic_states_;
  VkPipelineDynamicStateCreateInfo dynamic_state_{};

  VkGraphicsPipelineCreateInfo create_info_{};

  Device device_;
};

class Pipeline
{
  friend class GraphicsPipelineCreator;

public:
  Pipeline();

  explicit Pipeline(Device device);

  ~Pipeline();

  void Destroy();

  operator VkPipeline () { return pipeline_; }

private:
  Device device_;

  VkPipeline pipeline_ = NULL;
};
}
}

#endif // TESS_VK_VK_PIPELINE_H_
