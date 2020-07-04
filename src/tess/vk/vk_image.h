#ifndef TESS_VK_VK_IMAGE_H_
#define TESS_VK_VK_IMAGE_H_

#include <vulkan/vulkan.h>

namespace tess
{
namespace vk
{
class Image
{
public:
  Image();

  Image(VkImage image);

  ~Image();

  operator VkImage() { return image_; }

private:
  VkImage image_;
};
}
}

#endif // TESS_VK_VK_IMAGE_H_
