#include "tess/vk/vk_swapchain.h"

#include <iostream>

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
//
// SwapchainCreator
//
SwapchainCreator::SwapchainCreator(Device device, Surface surface)
  : device_(device), surface_(surface)
{
  create_info_.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info_.pNext = NULL;
  create_info_.flags = 0;

  create_info_.surface = surface;

  create_info_.imageArrayLayers = 1;
  create_info_.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  create_info_.oldSwapchain = VK_NULL_HANDLE;
}

SwapchainCreator::~SwapchainCreator()
{
}

void SwapchainCreator::LoadDeviceSwapchainProperties(PhysicalDevice physical_device)
{
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface_, &properties_.capabilities);

  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface_, &format_count, NULL);
  if (format_count != 0)
  {
    properties_.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface_, &format_count, properties_.formats.data());
  }
  else
    throw std::runtime_error("No available surface formats for swapchain!");

  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface_, &present_mode_count, nullptr);
  if (present_mode_count != 0)
  {
    properties_.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface_, &present_mode_count, properties_.present_modes.data());
  }
  else
    throw std::runtime_error("No available present modes for swapchain!");
}

void SwapchainCreator::PrintSwapchainProperties()
{
  std::cout << "Surface capabilities" << std::endl
    << "=======================" << std::endl
    << "\tminImageCount : " << properties_.capabilities.minImageCount << ", maxImageCount: " << properties_.capabilities.maxImageCount << std::endl
    << "\tcurrentExtent : (" << properties_.capabilities.currentExtent.width << ", " << properties_.capabilities.currentExtent.height << ")" << std::endl
    << "\tminImageExtent: (" << properties_.capabilities.minImageExtent.width << ", " << properties_.capabilities.minImageExtent.height << ")" << std::endl
    << "\tmaxImageExtent: (" << properties_.capabilities.maxImageExtent.width << ", " << properties_.capabilities.maxImageExtent.height << ")" << std::endl;

  std::cout << std::endl
    << "Surface formats" << std::endl
    << "=======================" << std::endl;

  for (const auto& format : properties_.formats)
    std::cout << format.format << " " << format.colorSpace << std::endl;
}

void SwapchainCreator::ChooseCurrentExtent()
{
  create_info_.imageExtent = properties_.capabilities.currentExtent;
  create_info_.preTransform = properties_.capabilities.currentTransform;
}

void SwapchainCreator::ChooseDefaultFormat()
{
  auto& format_to_use = properties_.formats[0];
  for (const auto& format : properties_.formats)
  {
    if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      format_to_use = format;
  }

  create_info_.imageFormat = format_to_use.format;
  create_info_.imageColorSpace = format_to_use.colorSpace;
  create_info_.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
}

void SwapchainCreator::ChooseFifoPresentMode()
{
  create_info_.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  create_info_.clipped = VK_TRUE;
}

void SwapchainCreator::EnableTrippleBuffering()
{
  uint32_t image_count = properties_.capabilities.minImageCount + 1;

  if (properties_.capabilities.maxImageCount > 0 && image_count > properties_.capabilities.maxImageCount)
    image_count = properties_.capabilities.maxImageCount;

  create_info_.minImageCount = image_count;
}

void SwapchainCreator::QueueFamilyExclusiveMode()
{
  create_info_.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
}

Swapchain SwapchainCreator::Create()
{
  Swapchain swapchain{ device_ };

  if (VkResult result = vkCreateSwapchainKHR(device_, &create_info_, NULL, &swapchain.swapchain_))
    throw Exception("Failed to create swap chain!", result);

  // Retrieve images
  uint32_t image_count;
  vkGetSwapchainImagesKHR(device_, swapchain, &image_count, nullptr);
  swapchain.images_.resize(image_count);
  vkGetSwapchainImagesKHR(device_, swapchain, &image_count, swapchain.images_.data());

  return swapchain;
}


//
// Swapchain
//
Swapchain::Swapchain()
{
}

Swapchain::Swapchain(Device device)
  : device_(device)
{
}

Swapchain::~Swapchain()
{
}

void Swapchain::Destroy()
{
  if (swapchain_)
  {
    vkDestroySwapchainKHR(device_, swapchain_, NULL);
    swapchain_ = NULL;
  }
}
}
}
