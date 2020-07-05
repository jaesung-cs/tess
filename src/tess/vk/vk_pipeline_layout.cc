#include "tess/vk/vk_pipeline_layout.h"

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
//
// PipelineLayoutCreator
//
PipelineLayoutCreator::PipelineLayoutCreator(Device device)
  : device_(device)
{
  create_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
}

PipelineLayoutCreator::~PipelineLayoutCreator()
{
}

PipelineLayout PipelineLayoutCreator::Create()
{
  PipelineLayout pipeline_layout{ device_ };

  if (VkResult result = vkCreatePipelineLayout(device_, &create_info_, NULL, &pipeline_layout.pipeline_layout_))
    throw Exception("Failed to create a pipeline layout!", result);

  return pipeline_layout;
}

//
// PipelineLayout
//
PipelineLayout::PipelineLayout()
{
}

PipelineLayout::PipelineLayout(Device device)
  : device_(device)
{
}

PipelineLayout::~PipelineLayout()
{
}

void PipelineLayout::Destroy()
{
  if (pipeline_layout_)
  {
    vkDestroyPipelineLayout(device_, pipeline_layout_, NULL);
    pipeline_layout_ = NULL;
  }
}
}
}
