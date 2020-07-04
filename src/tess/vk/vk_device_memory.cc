#include "tess/vk/vk_device_memory.h"

#include <cstring>

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
//
// DeviceMemoryAllocator
//
DeviceMemoryAllocator::DeviceMemoryAllocator(Device device)
  : device_(device)
{
  allocate_info_.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocate_info_.pNext = NULL;
}

DeviceMemoryAllocator::~DeviceMemoryAllocator()
{
}

void DeviceMemoryAllocator::ChooseHostVisibleCoherentMemory(PhysicalDevice physical_device, Buffer buffer)
{
  allocate_info_.memoryTypeIndex = physical_device.FindMemoryTypeIndex(buffer.MemoryRequirements(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

DeviceMemory DeviceMemoryAllocator::Allocate(int bytes)
{
  allocate_info_.allocationSize = bytes;

  DeviceMemory device_memory{ device_ };

  if (VkResult result = vkAllocateMemory(device_, &allocate_info_, NULL, &device_memory.device_memory_))
    throw Exception("Failed to allocate memory!", result);

  return device_memory;
}


//
// DeviceMemory
//
DeviceMemory::DeviceMemory()
{
}

DeviceMemory::DeviceMemory(Device device)
  : device_(device)
{
}

DeviceMemory::~DeviceMemory()
{
}

void DeviceMemory::Free()
{
  if (device_memory_)
  {
    vkFreeMemory(device_, device_memory_, NULL);
    device_memory_ = NULL;
  }
}

void DeviceMemory::MemoryCopy(int offset, int size, void* data)
{
  void* ptr;
  vkMapMemory(device_, device_memory_, offset, size, 0, &ptr);
  std::memcpy(ptr, data, size);
  vkUnmapMemory(device_, device_memory_);
}
}
}
