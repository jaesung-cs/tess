#include "tess/vk/vk_image.h"

namespace tess
{
namespace vk
{
Image::Image()
{
}

Image::Image(VkImage image)
  : image_(image)
{
}

Image::~Image()
{
}
}
}
