#include "tess/engine.h"

#include <iostream>
#include <fstream>
#include <set>
#include <cstdint>
#include <algorithm>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace tess
{
// GLFW Callback functions
namespace
{
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  auto engine = (Engine*)glfwGetWindowUserPointer(window);
  engine->Resize(width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  double x, y;
  glfwGetCursorPos(window, &x, &y);

  auto engine = (Engine*)glfwGetWindowUserPointer(window);
  engine->MouseButton(button, action, mods, x, y);
}

void cursor_pos_callback(GLFWwindow* window, double x, double y)
{
  auto engine = (Engine*)glfwGetWindowUserPointer(window);
  engine->MouseMove(x, y);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  auto engine = (Engine*)glfwGetWindowUserPointer(window);
  engine->Keyboard(key, action, mods);
}
}

// Utility functions
namespace
{
std::vector<char> ReadFile(const std::string& filename)
{
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open())
    throw std::runtime_error("failed to open file!");

  size_t file_size = (size_t)file.tellg();
  std::vector<char> buffer(file_size);

  file.seekg(0);
  file.read(buffer.data(), file_size);

  file.close();

  return buffer;
}
}

Engine::Engine() = default;

Engine::Engine(int width, int height)
  : width_(width), height_(height)
{
}

Engine::~Engine()
{
  glfwDestroyWindow(window_);
}

void Engine::Resize(int width, int height)
{
}

void Engine::MouseButton(int button, int action, int mods, double x, double y)
{
}

void Engine::MouseMove(double x, double y)
{
}

void Engine::Keyboard(int key, int action, int mods)
{
}

void Engine::Run()
{
  Initialize();

  while (!glfwWindowShouldClose(window_))
  {
    // Update events
    glfwPollEvents();
  }

  device_.WaitIdle();

  Cleanup();
}

void Engine::Initialize()
{
  InitializeGlfw();
  InitializeVulkan();
}

void Engine::InitializeGlfw()
{
  // GLFW init
  glfwInit();

  if (glfwVulkanSupported())
    std::cout << "Vulkan is supported" << std::endl;
  else
  {
    std::cout << "Vulkan is not supported" << std::endl;
    return;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // GLFW create window
  window_ = glfwCreateWindow(width_, height_, "tess", NULL, NULL);
  if (window_ == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return;
  }

  glfwMakeContextCurrent(window_);

  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
  glfwSetKeyCallback(window_, key_callback);
  glfwSetMouseButtonCallback(window_, mouse_button_callback);
  glfwSetCursorPosCallback(window_, cursor_pos_callback);
}

void Engine::InitializeVulkan()
{
  // Available layers and extensions
  layer_extension_.LoadLayerExtensions();
  layer_extension_.PrintLayerExtensions();

  // Prepare for instance creation
  vk::InstanceCreator instance_creator{ "Tess" };

  // Load validation layer
  if (enable_validation_layer_)
    instance_creator.EnableValidationLayer();

  // Load glfw required extensions
  uint32_t glfw_extension_count = 0;
  const char** glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  for (int i = 0; i < glfw_extension_count; i++)
    instance_creator.AddExtension(glfw_extensions[i]);

  // Create instance
  instance_ = instance_creator.Create();

  // Load device list
  vk::DeviceList device_list(instance_);
  device_list.PrintDeviceExtensionProperties();
  device_list.PrintDeviceQueueFamilies();

  // Pick physical device
  physical_device_ = device_list.SelectBestGraphicsDevice();

  // Create logical device
  vk::DeviceCreator device_creator(physical_device_);
  device_creator.AddExtension(vk::DeviceExtension::KHR_SWAPCHAIN);
  device_creator.AddGraphicsQueue();

  device_ = device_creator.Create();

  // Get queues from device
  graphics_queue_ = device_.GetQueue(0);
}

void Engine::Cleanup()
{
  device_.Destroy();
  instance_.Destroy();

  glfwDestroyWindow(window_);
  glfwTerminate();
}
}
