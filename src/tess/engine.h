#ifndef TESS_ENGINE_H_
#define TESS_ENGINE_H_

#include <memory>
#include <vector>
#include <optional>

#include <vulkan/vulkan.hpp>

#include "tess/vk/vk_layer_extension.h"
#include "tess/vk/vk_instance.h"
#include "tess/vk/vk_device.h"
#include "tess/vk/vk_queue.h"
#include "tess/vk/vk_surface.h"
#include "tess/vk/vk_swapchain.h"
#include "tess/vk/vk_buffer.h"
#include "tess/vk/vk_device_memory.h"
#include "tess/vk/vk_shader_module.h"
#include "tess/vk/vk_render_pass.h"
#include "tess/vk/vk_pipeline_layout.h"
#include "tess/vk/vk_pipeline.h"
#include "tess/vk/vk_framebuffer.h"
#include "tess/vk/vk_command_buffer.h"
#include "tess/vk/vk_semaphore.h"
#include "tess/vk/vk_fence.h"

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

  void DrawFrame();

  void Cleanup();

  GLFWwindow* window_ = nullptr;

  // Vulkan wrapper
  vk::LayerExtension layer_extension_;
  vk::Instance instance_;

  vk::Surface surface_;

  vk::PhysicalDevice physical_device_;
  vk::Device device_;
  vk::Queue graphics_queue_;
  vk::Queue present_queue_;

  vk::Swapchain swapchain_;

  vk::Buffer buffer_;
  vk::DeviceMemory device_memory_;

  vk::ShaderModule vertex_shader_;
  vk::ShaderModule fragment_shader_;

  vk::RenderPass render_pass_;

  vk::PipelineLayout pipeline_layout_;

  vk::Pipeline pipeline_;

  std::vector<vk::Framebuffer> swapchain_framebuffers_;

  vk::CommandPool command_pool_;

  std::vector<vk::CommandBuffer> swapchain_command_buffers_;

  // Frames in flight
  static const int MAX_FRAMES_IN_FLIGHT = 2;
  int current_frame_ = 0;

  std::vector<vk::Semaphore> image_available_semaphore_;
  std::vector<vk::Semaphore> render_finished_semaphore_;
  std::vector<vk::Fence> in_flight_fences_;
  std::vector<vk::Fence> images_in_flight_fences_;

  // Validation layer for debug
#ifdef NDEBUG
  const bool enable_validation_layer_ = false;
#else
  const bool enable_validation_layer_ = true;
#endif
};
}

#endif // TESS_ENGINE_H_
