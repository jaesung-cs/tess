#include "tess/vk/vk_framebuffer.h"

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
//
// FramebufferCreator
//
FramebufferCreator::FramebufferCreator(Device device)
  : device_(device)
{
  create_info_.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

  create_info_.layers = 1;
}

FramebufferCreator::~FramebufferCreator()
{
}

void FramebufferCreator::AddAttachment(const ImageView& image_view)
{
  attachments_.push_back(image_view);
}

void FramebufferCreator::SetRenderPass(const RenderPass& render_pass)
{
  create_info_.renderPass = render_pass;
}

void FramebufferCreator::SetSize(int width, int height)
{
  create_info_.width = width;
  create_info_.height = height;
}

Framebuffer FramebufferCreator::Create()
{
  create_info_.attachmentCount = attachments_.size();
  create_info_.pAttachments = attachments_.data();

  Framebuffer framebuffer{ device_ };

  if (VkResult result = vkCreateFramebuffer(device_, &create_info_, NULL, &framebuffer.framebuffer_))
    throw Exception("Failed to create a framebuffer!", result);

  return framebuffer;
}


//
// Framebuffer
//
Framebuffer::Framebuffer()
{
}

Framebuffer::Framebuffer(Device device)
  : device_(device)
{
}

Framebuffer::~Framebuffer()
{
}

void Framebuffer::Destroy()
{
  if (framebuffer_)
  {
    vkDestroyFramebuffer(device_, framebuffer_, NULL);
    framebuffer_ = NULL;
  }
}
}
}
