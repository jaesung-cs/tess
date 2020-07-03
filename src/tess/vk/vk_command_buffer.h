#ifndef TESS_VK_VK_COMMAND_BUFFER_H_
#define TESS_VK_VK_COMMAND_BUFFER_H_

#include <vulkan/vulkan.h>

#include "tess/vk/vk_device.h"

namespace tess
{
namespace vk
{
class CommandPool
{
public:
  CommandPool();
  CommandPool(Device device);
  ~CommandPool();

  void Reset();
  void Destroy();

private:
  Device device_;

  VkCommandPool command_pool_ = NULL;
};

class CommandBuffer
{
public:
  CommandBuffer();
  ~CommandBuffer();

private:
};
}
}

#endif // TESS_VK_VK_COMMAND_BUFFER_H_
