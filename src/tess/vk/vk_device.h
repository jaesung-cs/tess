#ifndef TESS_VK_VK_DEVICE_H_
#define TESS_VK_VK_DEVICE_H_

#include <array>
#include <map>

#include <vulkan/vulkan.h>

#include "tess/vk/vk_instance.h"
#include "tess/vk/vk_queue.h"
#include "tess/vk/vk_surface.h"

namespace tess
{
namespace vk
{
class PhysicalDevice;
class Device;

class DeviceList
{
public:
  DeviceList();

  explicit DeviceList(Instance instance);

  ~DeviceList();

  void PrintDeviceExtensionProperties();
  void PrintDeviceQueueFamilies();
  void PrintMemoryProperties();

  void SetExtensions(const std::vector<const char*>& extensions);

  PhysicalDevice SelectGraphicsDevice(int device_index);
  PhysicalDevice SelectBestGraphicsDevice();

private:
  Instance instance_;

  std::vector<PhysicalDevice> physical_devices_;

  VkDeviceQueueCreateInfo queue_info_{};
  VkDeviceCreateInfo device_info_{};
};

class PhysicalDevice
{
public:
  PhysicalDevice();

  PhysicalDevice(Instance instance, VkPhysicalDevice physical_device);

  ~PhysicalDevice();

  operator VkPhysicalDevice ()
  {
    return physical_device_;
  }

  void LoadExtensions();
  void LoadQueueFamilies();

  void PrintDeviceExtensionProperties();
  void PrintDeviceQueueFamilies();

  bool IsSupportedExtension(const std::string& extension_name);

  void LoadMemoryProperties();
  void PrintMemoryProperties();

  uint32_t FindMemoryTypeIndex(VkMemoryRequirements memory_requirements, VkFlags property_flags);

  const auto& QueueFamilies() const { return queue_families_; }

private:
  Instance instance_;
  VkPhysicalDevice physical_device_ = NULL;

  // Device info
  VkPhysicalDeviceMemoryProperties memory_properties_{};
  std::vector<VkExtensionProperties> extensions_;
  std::unordered_set<std::string> extension_names_;

  std::vector<VkQueueFamilyProperties> queue_families_;
};

class DeviceCreator
{
private:
  enum class QueueType
  {
    GRAPHICS,
    COMPUTE,
    TRANSFER,
    SPARSE_BINDING,
  };

public:
  DeviceCreator() = delete;

  explicit DeviceCreator(PhysicalDevice physical_device);

  ~DeviceCreator();

  void AddExtension(DeviceExtension extension);
  void AddExtension(const std::string& extension_name);

  void AddGraphicsQueue();
  void AddPresentQueue(Surface surface);

  Device Create();

private:
  std::vector<std::string> extensions_;

  VkPhysicalDeviceFeatures physical_device_features_{};
  VkDeviceCreateInfo create_info_{};

  std::vector<int> queue_family_count_;

  struct QueueFamilyIndex
  {
    QueueFamilyIndex(int family_index, int queue_index)
      : family_index(family_index), queue_index(queue_index) {}

    int family_index;
    int queue_index;
  };

  std::vector<QueueFamilyIndex> queue_family_indices_;

  Instance instance_;
  PhysicalDevice physical_device_;
};

class Device
{
  friend class DeviceCreator;

public:
  Device();

  ~Device();

  void Destroy();

  operator VkDevice () { return device_; }

  void WaitIdle();

  auto GetQueue(int index) { return queues_[index]; }

private:
  VkDevice device_ = NULL;

  std::vector<Queue> queues_;
};
}
}

#endif // TESS_VK_VK_DEVICE_H_
