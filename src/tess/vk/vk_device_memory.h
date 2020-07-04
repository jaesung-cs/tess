#ifndef TESS_VK_VK_DEVICE_MEMORY_H_
#define TESS_VK_VK_DEVICE_MEMORY_H_

#include <vulkan/vulkan.h>

#include "tess/vk/vk_device.h"
#include "tess/vk/vk_buffer.h"

namespace tess
{
namespace vk
{
class DeviceMemory;

class DeviceMemoryAllocator
{
public:
  DeviceMemoryAllocator() = delete;

  explicit DeviceMemoryAllocator(Device device);

  ~DeviceMemoryAllocator();

  void ChooseHostVisibleCoherentMemory(PhysicalDevice physical_device, Buffer buffer);

  DeviceMemory Allocate(int bytes);

private:
  VkMemoryAllocateInfo allocate_info_{};

  Device device_;
};

class DeviceMemory
{
  friend class DeviceMemoryAllocator;

public:
  DeviceMemory();

  explicit DeviceMemory(Device device);

  ~DeviceMemory();

  void Free();

  operator VkDeviceMemory () { return device_memory_; }

  void MemoryCopy(int offset, int size, void* data);

private:
  Device device_;

  VkDeviceMemory device_memory_ = NULL;
};
}
}

#endif // TESS_VK_VK_DEVICE_MEMORY_H_
