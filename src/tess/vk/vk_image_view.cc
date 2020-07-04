#include "tess/vk/vk_image_view.h"

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
//
// ImageViewCreator
//
ImageViewCreator::ImageViewCreator(Device device, Image image)
  : device_(device)
{
  create_info_.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  create_info_.image = image;
  create_info_.flags = 0;

  create_info_.viewType = VK_IMAGE_VIEW_TYPE_2D;

  SwizzleIdentity();
  DisableMipmap();
}

ImageViewCreator::~ImageViewCreator()
{
}

void ImageViewCreator::SetImageFormat(VkFormat format)
{
  create_info_.format = format;
}

void ImageViewCreator::SwizzleIdentity()
{
  create_info_.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info_.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info_.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info_.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
}

void ImageViewCreator::DisableMipmap()
{
  create_info_.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  create_info_.subresourceRange.baseMipLevel = 0;
  create_info_.subresourceRange.levelCount = 1;
  create_info_.subresourceRange.baseArrayLayer = 0;
  create_info_.subresourceRange.layerCount = 1;
}

ImageView ImageViewCreator::Create()
{
  ImageView image_view{ device_ };

  if (VkResult result = vkCreateImageView(device_, &create_info_, NULL, &image_view.image_view_))
    throw Exception("Failed to create image views!", result);

  return image_view;
}

//
// ImageView
//
ImageView::ImageView()
{
}

ImageView::ImageView(Device device)
  : device_(device)
{
}

ImageView::~ImageView()
{
}

void ImageView::Destroy()
{
  if (image_view_)
  {
    vkDestroyImageView(device_, image_view_, NULL);
    image_view_ = NULL;
  }
}
}
}
