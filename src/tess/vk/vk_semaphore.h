#ifndef TESS_VK_VK_SEMAPHORE_H_
#define TESS_VK_VK_SEMAPHORE_H_

#include <vulkan/vulkan.h>

#include "tess/vk/vk_device.h"

namespace tess
{
namespace vk
{
class Semaphore;

class SemaphoreCreator
{
public:
  SemaphoreCreator() = delete;

  explicit SemaphoreCreator(Device device);

  ~SemaphoreCreator();

  Semaphore Create();

private:
  Device device_;

  VkSemaphoreCreateInfo create_info_{};
};

class Semaphore
{
  friend class SemaphoreCreator;

public:
  Semaphore();

  explicit Semaphore(Device device);

  ~Semaphore();

  void Destroy();

  operator VkSemaphore () { return semaphore_; }

private:
  Device device_;

  VkSemaphore semaphore_ = NULL;
};
}
}

#endif // TESS_VK_VK_SEMAPHORE_H_
