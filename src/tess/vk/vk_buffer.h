#ifndef TESS_VK_VK_BUFFER_H_
#define TESS_VK_VK_BUFFER_H_

#include <vulkan/vulkan.h>

#include "tess/vk/vk_device.h"

namespace tess
{
namespace vk
{
class Buffer;

class BufferCreator
{
public:
  BufferCreator() = delete;

  explicit BufferCreator(Device device);

  ~BufferCreator();

  void UseAsVertexBuffer();

  Buffer Create(int bytes);

private:
  VkBufferCreateInfo create_info_{};

  Device device_;
};

class Buffer
{
  friend class BufferCreator;

public:
  Buffer();

  explicit Buffer(Device device);

  ~Buffer();

  void Destroy();

  operator VkBuffer () { return buffer_; }

  void LoadMemoryRequirements();
  const auto& MemoryRequirements() const { return memory_requirements_; }

private:
  Device device_;

  VkBuffer buffer_ = NULL;

  VkMemoryRequirements memory_requirements_{};
};
}
}

#endif // TESS_VK_VK_DEVICE_MEMORY_H_
