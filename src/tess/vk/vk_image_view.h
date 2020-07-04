#ifndef TESS_VK_VK_IMAGE_VIEW_H_
#define TESS_VK_VK_IMAGE_VIEW_H_

#include <vulkan/vulkan.h>

#include "tess/vk/vk_device.h"
#include "tess/vk/vk_image.h"

namespace tess
{
namespace vk
{
class ImageView;

class ImageViewCreator
{
public:
  ImageViewCreator() = delete;

  ImageViewCreator(Device device, Image image);

  ~ImageViewCreator();

  void SetImageFormat(VkFormat format);
  void SwizzleIdentity();
  void DisableMipmap();

  ImageView Create();

private:
  Device device_;

  VkImageViewCreateInfo create_info_{};
};

class ImageView
{
  friend class ImageViewCreator;

public:
  ImageView();

  ImageView(Device device_);

  ~ImageView();

  void Destroy();

  operator VkImageView() { return image_view_; }

private:
  Device device_;

  VkImageView image_view_ = NULL;
};
}
}

#endif // TESS_VK_VK_IMAGE_VIEW_H_
