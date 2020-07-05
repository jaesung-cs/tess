#ifndef TESS_VK_VK_SWAPCHAIN_H_
#define TESS_VK_VK_SWAPCHAIN_H_

#include <vulkan/vulkan.h>

#include "tess/vk/vk_device.h"
#include "tess/vk/vk_surface.h"
#include "tess/vk/vk_image.h"
#include "tess/vk/vk_image_view.h"
#include "tess/vk/vk_semaphore.h"

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
    std::vector<VkSurfaceFormatKHR> surface_formats;
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

  auto ImageFormat() const { return image_format_; }
  const auto& ImageViews() const { return image_views_; }

  uint32_t AcquireNextImage(Semaphore semaphore);

private:
  void CreateImageViews();

  Device device_;

  VkSwapchainKHR swapchain_ = NULL;

  // Swapchain info
  VkFormat image_format_;

  std::vector<VkImage> images_;
  std::vector<ImageView> image_views_;
};
}
}

#endif // TESS_VK_VK_SWAPCHAIN_H_
