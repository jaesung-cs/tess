#ifndef TESS_VK_VK_DEVICE_H_
#define TESS_VK_VK_DEVICE_H_

#include <vulkan/vulkan.h>

#include "tess/vk/vk_instance.h"
#include "tess/vk/vk_layer_extension.h"

namespace tess
{
namespace vk
{
class Device;

class Queue
{
  friend class DeviceList;

public:
  Queue();
  ~Queue();

  operator VkQueue ()
  {
    return queue_;
  }

private:
  VkQueue queue_;
};

class DeviceList
{
public:
  DeviceList();

  DeviceList(Instance instance);

  ~DeviceList();

  void PrintDeviceExtensionProperties();
  void PrintDeviceQueueFamilies();

  void SetExtensions(const std::vector<const char*>& extensions);

  Device SelectGraphicsDevice(int device_index);
  Device SelectBestGraphicsDevice();

private:
  Instance instance_;

  std::vector<VkPhysicalDevice> physical_devices_;

  VkDeviceQueueCreateInfo queue_info_{};
  VkDeviceCreateInfo device_info_{};
};

class Device
{
  friend class DeviceList;

public:
  Device();

  Device(Instance instance);

  ~Device();

  void Destroy();

  operator VkDevice ()
  {
    return device_;
  }

  operator VkPhysicalDevice ()
  {
    return physical_device_;
  }

  void WaitIdle();

  auto GraphicsQueue() const
  {
    return graphics_queue_;
  }

private:
  Instance instance_;

  VkDevice device_ = NULL;
  VkPhysicalDevice physical_device_ = NULL;

  // Queue indices
  uint32_t queue_family_count_ = 0;
  uint32_t graphics_queue_family_index_ = 0;

  // Queues
  Queue graphics_queue_;
};
}
}

#endif // TESS_VK_VK_DEVICE_H_
