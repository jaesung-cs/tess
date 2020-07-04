#ifndef TESS_VK_VK_SWAPCHAIN_H_
#define TESS_VK_VK_SWAPCHAIN_H_

#include <vulkan/vulkan.h>

#include "tess/vk/vk_device.h"
#include "tess/vk/vk_surface.h"

namespace tess
{
namespace vk
{
class Swapchain;

class SwapchainCreator
{
public:
  SwapchainCreator() = delete;

  explicit SwapchainCreator(Device device, Surface surface);

  ~SwapchainCreator();

  void LoadDeviceSwapchainProperties(PhysicalDevice physical_device);

  void PrintSwapchainProperties();

  // Extent
  void ChooseCurrentExtent();

  // Choose BGRA8 SRGB color format
  void ChooseDefaultFormat();

  // Present mode
  void ChooseFifoPresentMode();

  // Double/tripple buffering
  void EnableTrippleBuffering();

  void QueueFamilyExclusiveMode();

  Swapchain Create();

private:
  struct SwapchainProperties
  {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
  };

  SwapchainProperties properties_;

  VkSwapchainCreateInfoKHR create_info_{};

  Device device_;
  Surface surface_;
};

class Swapchain
{
  friend class SwapchainCreator;

public:
  Swapchain();

  explicit Swapchain(Device device);

  ~Swapchain();

  void Destroy();

  operator VkSwapchainKHR() { return swapchain_; }

private:
  Device device_;

  VkSwapchainKHR swapchain_ = NULL;

  std::vector<VkImage> images_;
};
}
}

#endif // TESS_VK_VK_SWAPCHAIN_H_
