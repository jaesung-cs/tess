#ifndef TESS_VK_VK_SURFACE_H_
#define TESS_VK_VK_SURFACE_H_

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "tess/vk/vk_instance.h"

namespace tess
{
namespace vk
{
class Surface;

class GlfwSurfaceCreator
{
public:
  GlfwSurfaceCreator() = delete;

  GlfwSurfaceCreator(Instance instance, GLFWwindow* window);

  ~GlfwSurfaceCreator();

  Surface Create();

private:
  Instance instance_;
  GLFWwindow* window_;
};

class Surface
{
  friend class GlfwSurfaceCreator;

public:
  Surface();

  explicit Surface(Instance instance);

  ~Surface();

  void Destroy();

  operator VkSurfaceKHR() { return surface_; }

private:
  Instance instance_;

  VkSurfaceKHR surface_ = NULL;
};
}
}

#endif // TESS_VK_VK_SURFACE_H_
