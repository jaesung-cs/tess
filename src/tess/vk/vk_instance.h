#ifndef TESS_VK_VK_INSTANCE_H_
#define TESS_VK_VK_INSTANCE_H_

#include <vulkan/vulkan.hpp>

#include "tess/vk/vk_object.h"

namespace tess
{
namespace vk
{
class Instance;

class InstanceCreator
{
public:
  InstanceCreator()
  {
    app_info_.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    info_.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info_.pApplicationInfo = &app_info_;
  }

  ~InstanceCreator() = default;

  Instance Create();

  auto& Info() { return info_; }
  const auto& Info() const { return info_; }

  auto& ApplicationInfo() { return app_info_; }
  const auto& ApplicationInfo() const { return app_info_; }

private:
  VkApplicationInfo app_info_{};
  VkInstanceCreateInfo info_{};
};

class Instance : public Object
{
  friend class InstanceCreator;

public:
  static VkDebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo(void* user_data = nullptr);

public:
  Instance();
  ~Instance() override;

  Instance(const Instance& rhs) = delete;
  Instance& operator = (const Instance& rhs) = delete;

  Instance(Instance&& rhs) noexcept
  {
    instance_ = rhs.instance_;
    rhs.instance_ = nullptr;
  }

  Instance& operator = (Instance&& rhs) noexcept
  {
    instance_ = rhs.instance_;
    rhs.instance_ = nullptr;

    return *this;
  }
  
  operator VkInstance()
  {
    return instance_;
  }

  operator const VkInstance() const
  {
    return instance_;
  }

  void SetupDebugMessenger();

private:
  void Destroy() override;

  VkInstance instance_ = nullptr;
  VkDebugUtilsMessengerEXT debug_messenger_ = nullptr;
};
}
}

#endif // TESS_VK_VK_INSTANCE_H_
