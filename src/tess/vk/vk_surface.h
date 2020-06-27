#ifndef TESS_VK_VK_SURFACE_H_
#define TESS_VK_VK_SURFACE_H_

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "tess/vk/vk_instance.h"
#include "tess/vk/vk_object.h"

namespace tess
{
namespace vk
{
class Surface;

class SurfaceCreator
{
public:
  SurfaceCreator(vk::Instance& instance, GLFWwindow* window)
    : instance_(instance), window_(window)
  {
  }

  ~SurfaceCreator() = default;

  Surface Create();

private:
  vk::Instance& instance_;
  GLFWwindow* window_ = nullptr;
};

class Surface : public Object
{
  friend class SurfaceCreator;

public:
  Surface();
  ~Surface() override;

  Surface(const Surface& rhs) = delete;
  Surface& operator = (const Surface& rhs) = delete;

  Surface(Surface&& rhs) noexcept
  {
    instance_ = rhs.instance_;
    surface_ = rhs.surface_;

    rhs.instance_ = nullptr;
    rhs.surface_ = nullptr;
  }

  Surface& operator = (Surface&& rhs) noexcept
  {
    instance_ = rhs.instance_;
    surface_ = rhs.surface_;

    rhs.instance_ = nullptr;
    rhs.surface_ = nullptr;

    return *this;
  }
  
  operator VkSurfaceKHR()
  {
    return surface_;
  }

  void Destroy() override;

private:
  const vk::Instance* instance_ = nullptr;
  VkSurfaceKHR surface_ = nullptr;
};
}
}

#endif // TESS_VK_VK_SURFACE_H_
