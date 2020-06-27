#include "tess/vk/vk_surface.h"

#include <iostream>

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
Surface SurfaceCreator::Create()
{
  Surface surface;

  // TODO: allow allocator
  instance_.AddDependency(&surface);
  surface.instance_ = &instance_;
  auto result = glfwCreateWindowSurface(instance_, window_, nullptr, &surface.surface_);
  if (result != VK_SUCCESS)
    throw Exception("failed to create window surface!", result);

  return surface;
}

Surface::Surface() = default;

Surface::~Surface()
{
  Destroy();
}

void Surface::Destroy()
{
  if (surface_)
  {
    vkDestroySurfaceKHR(*instance_, surface_, nullptr);
    surface_ = nullptr;
    instance_ = nullptr;
  }
}
}
}
