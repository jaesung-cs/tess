#include "tess/vk/vk_render_pass.h"

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
//
// RenderPassCreator
//
RenderPassCreator::RenderPassCreator(Device device)
  : device_(device)
{
  // Color attachment
  color_attachment_.samples = VK_SAMPLE_COUNT_1_BIT;

  color_attachment_.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment_.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

  color_attachment_.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

  color_attachment_.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment_.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  // Color attachment reference
  color_attachment_reference_.attachment = 0;
  color_attachment_reference_.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // Subpass
  subpass_.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  subpass_.colorAttachmentCount = 1;
  subpass_.pColorAttachments = &color_attachment_reference_;

  // Dependency
  dependency_.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency_.dstSubpass = 0;

  dependency_.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency_.srcAccessMask = 0;

  dependency_.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency_.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  // Render pass
  create_info_.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  create_info_.pNext = NULL;

  create_info_.attachmentCount = 1;
  create_info_.pAttachments = &color_attachment_;

  create_info_.subpassCount = 1;
  create_info_.pSubpasses = &subpass_;

  create_info_.dependencyCount = 1;
  create_info_.pDependencies = &dependency_;
}

RenderPassCreator::~RenderPassCreator()
{
}

void RenderPassCreator::SetColorAttachmentImageFormat(VkFormat image_format)
{
  color_attachment_.format = image_format;
}

RenderPass RenderPassCreator::Create()
{
  RenderPass render_pass{ device_ };

  if (VkResult result = vkCreateRenderPass(device_, &create_info_, NULL, &render_pass.render_pass_))
    throw Exception("Failed to create a render pass!", result);

  return render_pass;
}


//
// RenderPass
//
RenderPass::RenderPass()
{
}

RenderPass::RenderPass(Device device)
  : device_(device)
{
}

RenderPass::~RenderPass()
{
}

void RenderPass::Destroy()
{
  if (render_pass_)
  {
    vkDestroyRenderPass(device_, render_pass_, NULL);
    render_pass_ = NULL;
  }
}
}
}
