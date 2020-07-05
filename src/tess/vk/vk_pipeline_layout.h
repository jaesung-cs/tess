#ifndef TESS_VK_VK_PIPELINE_LAYOUT_H_
#define TESS_VK_VK_PIPELINE_LAYOUT_H_

#include <vulkan/vulkan.h>

#include "tess/vk/vk_device.h"

namespace tess
{
namespace vk
{
class PipelineLayout;

class PipelineLayoutCreator
{
public:
  PipelineLayoutCreator() = delete;

  explicit PipelineLayoutCreator(Device device);

  ~PipelineLayoutCreator();

  PipelineLayout Create();

private:
  VkPipelineLayoutCreateInfo create_info_{};

  Device device_;
};

class PipelineLayout
{
  friend class PipelineLayoutCreator;

public:
  PipelineLayout();

  explicit PipelineLayout(Device device);

  ~PipelineLayout();

  void Destroy();

  operator VkPipelineLayout () { return pipeline_layout_; }

private:
  Device device_;

  VkPipelineLayout pipeline_layout_ = NULL;
};
}
}

#endif // TESS_VK_VK_PIPELINE_LAYOUT_H_
