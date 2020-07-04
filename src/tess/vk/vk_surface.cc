#include "tess/vk/vk_surface.h"

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
//
// GlfwSurfaceCreator
//
GlfwSurfaceCreator::GlfwSurfaceCreator(Instance instance, GLFWwindow* window)
  : instance_(instance), window_(window)
{
}

GlfwSurfaceCreator::~GlfwSurfaceCreator()
{
}

Surface GlfwSurfaceCreator::Create()
{
  Surface surface{ instance_ };

  if (VkResult result = glfwCreateWindowSurface(instance_, window_, nullptr, &surface.surface_))
    throw Exception("failed to create window surface!", result);

  return surface;
}


//
// Surface
//
Surface::Surface()
{
}

Surface::Surface(Instance instance)
  : instance_(instance)
{
}

Surface::~Surface()
{
}

void Surface::Destroy()
{
  if (surface_)
  {
    vkDestroySurfaceKHR(instance_, surface_, NULL);
    surface_ = NULL;
  }
}
}
}
