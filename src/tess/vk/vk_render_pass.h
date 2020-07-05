#ifndef TESS_VK_VK_RENDER_PASS_H_
#define TESS_VK_VK_RENDER_PASS_H_

#include <vulkan/vulkan.h>

#include "tess/vk/vk_device.h"

namespace tess
{
namespace vk
{
class RenderPass;

class RenderPassCreator
{
public:
  RenderPassCreator() = delete;

  explicit RenderPassCreator(Device device);

  ~RenderPassCreator();

  void SetColorAttachmentImageFormat(VkFormat image_format);

  RenderPass Create();

private:
  Device device_;

  VkAttachmentDescription color_attachment_{};
  VkAttachmentReference color_attachment_reference_{};
  VkSubpassDescription subpass_{};
  VkSubpassDependency dependency_{};

  VkRenderPassCreateInfo create_info_{};
};

class RenderPass
{
  friend class RenderPassCreator;

public:
  RenderPass();

  explicit RenderPass(Device device_);

  ~RenderPass();

  void Destroy();

  operator VkRenderPass() const { return render_pass_; }

private:
  Device device_;

  VkRenderPass render_pass_ = NULL;
};
}
}

#endif // TESS_VK_VK_RENDER_PASS_H_
