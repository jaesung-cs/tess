#ifndef TESS_VK_VK_QUEUE_H_
#define TESS_VK_VK_QUEUE_H_

#include <vulkan/vulkan.h>

namespace tess
{
namespace vk
{
class Queue
{
  friend class DeviceCreator;

public:
  Queue();
  ~Queue();

  operator VkQueue () { return queue_; }

private:
  VkQueue queue_;
};
}
}

#endif // TESS_VK_VK_QUEUE_H_
