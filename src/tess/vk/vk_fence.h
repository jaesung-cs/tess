#ifndef TESS_VK_VK_FENCE_H_
#define TESS_VK_VK_FENCE_H_

#include <vulkan/vulkan.h>

#include "tess/vk/vk_device.h"

namespace tess
{
namespace vk
{
class Fence;

class FenceCreator
{
public:
  FenceCreator() = delete;

  explicit FenceCreator(Device device);

  ~FenceCreator();

  Fence Create();

private:
  Device device_;

  VkFenceCreateInfo create_info_{};
};

class Fence
{
  friend class FenceCreator;

public:
  Fence();

  explicit Fence(Device device);

  ~Fence();

  void Destroy();

  operator VkFence () { return fence_; }

  bool IsValid();

  void Wait();
  void Reset();

private:
  Device device_;

  VkFence fence_ = VK_NULL_HANDLE;
};
}
}

#endif // TESS_VK_VK_FENCE_H_
