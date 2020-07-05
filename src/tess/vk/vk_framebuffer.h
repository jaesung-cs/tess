#ifndef TESS_VK_VK_FRAMEBUFFER_H_
#define TESS_VK_VK_FRAMEBUFFER_H_

#include <vulkan/vulkan.h>

#include "tess/vk/vk_device.h"
#include "tess/vk/vk_image_view.h"
#include "tess/vk/vk_render_pass.h"

namespace tess
{
namespace vk
{
class Framebuffer;

class FramebufferCreator
{
public:
  FramebufferCreator() = delete;

  explicit FramebufferCreator(Device device);

  ~FramebufferCreator();

  void AddAttachment(const ImageView& image_view);
  void SetRenderPass(const RenderPass& render_pass);
  void SetSize(int width, int height);

  Framebuffer Create();

private:
  Device device_;

  std::vector<VkImageView> attachments_;

  VkFramebufferCreateInfo create_info_{};
};

class Framebuffer
{
  friend class FramebufferCreator;

public:
  Framebuffer();

  explicit Framebuffer(Device device);

  ~Framebuffer();

  void Destroy();

  operator VkFramebuffer () { return framebuffer_; }

private:
  Device device_;

  VkFramebuffer framebuffer_ = NULL;
};
}
}

#endif // TESS_VK_VK_FRAMEBUFFER_H_
