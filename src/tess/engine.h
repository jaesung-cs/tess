#ifndef TESS_ENGINE_H_
#define TESS_ENGINE_H_

#include <memory>
#include <vector>
#include <optional>

#include <vulkan/vulkan.hpp>

#include "tess/vk/vk_layer_extension.h"
#include "tess/vk/vk_instance.h"
#include "tess/vk/vk_device.h"

struct GLFWwindow;

namespace tess
{
class Engine
{
public:
  Engine();

  Engine(int width, int height);

  ~Engine();

  void Resize(int width, int height);
  void MouseButton(int button, int action, int mods, double x, double y);
  void MouseMove(double x, double y);
  void Keyboard(int key, int action, int mods);

  void Run();

private:
  int width_ = 1600;
  int height_ = 900;

  void Initialize();
  void InitializeGlfw();
  void InitializeVulkan();

  void Cleanup();

  std::vector<const char*> GetRequiredExtensions();

  GLFWwindow* window_ = nullptr;
  size_t current_frame_ = 0;

  // Vulkan wrapper
  vk::LayerExtension layer_extension_;
  vk::Instance instance_;
  vk::Device device_;
  vk::Queue graphics_queue_;
  vk::Queue present_queue_;
};
}

#endif // TESS_ENGINE_H_
