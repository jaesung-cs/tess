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

    // Draw
    DrawFrame();
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

  // Create surface
  vk::GlfwSurfaceCreator surface_creator{ instance_, window_ };
  surface_ = surface_creator.Create();

  // Load device list
  vk::DeviceList device_list{ instance_ };
  device_list.PrintDeviceExtensionProperties();
  device_list.PrintDeviceQueueFamilies();
  device_list.PrintMemoryProperties();

  // Pick physical device
  physical_device_ = device_list.SelectBestGraphicsDevice();

  // Create logical device
  vk::DeviceCreator device_creator{ physical_device_ };
  device_creator.AddExtension(vk::DeviceExtension::KHR_SWAPCHAIN);
  device_creator.AddGraphicsQueue();
  device_creator.AddPresentQueue(surface_);
  device_ = device_creator.Create();

  // Get queues from device
  graphics_queue_ = device_.GetQueue(0);
  present_queue_ = device_.GetQueue(1);

  // Create swapchain
  vk::SwapchainCreator swapchain_creator{ device_, surface_ };
  swapchain_creator.LoadDeviceSwapchainProperties(physical_device_);
  swapchain_creator.PrintSwapchainProperties();
  swapchain_creator.ChooseCurrentExtent();
  swapchain_creator.ChooseDefaultFormat();
  swapchain_creator.ChooseFifoPresentMode();
  swapchain_creator.EnableTrippleBuffering();
  swapchain_creator.QueueFamilyExclusiveMode();
  swapchain_ = swapchain_creator.Create();

  // Create vertex buffer
  vk::BufferCreator buffer_creator{ device_ };
  buffer_creator.UseAsVertexBuffer();
  buffer_ = buffer_creator.Create(1048576);

  // Allocate memory chunk
  // TODO: allocate memory first, then create buffer
  vk::DeviceMemoryAllocator device_memory_allocator{ device_ };
  device_memory_allocator.ChooseHostVisibleCoherentMemory(physical_device_, buffer_);
  device_memory_ = device_memory_allocator.Allocate(1048576);

  // Filling buffer
  float data[] =
  {
    0.0f, -0.5f, 1.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
  };
  device_memory_.MemoryCopy(0, sizeof(float) * 15, data);

  // Create shaders
  vk::ShaderModuleCreator shader_module_creator{ device_ };
  shader_module_creator.LoadCompiledSpirVFile("..\\src\\shader\\vert.spv");
  vertex_shader_ = shader_module_creator.Create();
  shader_module_creator.LoadCompiledSpirVFile("..\\src\\shader\\frag.spv");
  fragment_shader_ = shader_module_creator.Create();

  // Create render pass
  vk::RenderPassCreator render_pass_creator{ device_ };
  render_pass_creator.SetColorAttachmentImageFormat(swapchain_.ImageFormat());
  render_pass_ = render_pass_creator.Create();

  // Create pipeline layout
  vk::PipelineLayoutCreator pipeline_layout_creator{ device_ };
  pipeline_layout_ = pipeline_layout_creator.Create();

  // Create pipeline
  vk::GraphicsPipelineCreator pipeline_creator{ device_ };
  pipeline_creator.AddVertexShaderStage(vertex_shader_);
  pipeline_creator.AddFragmentShaderStage(fragment_shader_);
  pipeline_creator.SetViewport(width_, height_);
  pipeline_creator.SetPipelineLayout(pipeline_layout_);
  pipeline_creator.SetRenderPass(render_pass_);
  pipeline_ = pipeline_creator.Create();

  // Create framebuffers for each swapchain image
  for (const auto& image_view : swapchain_.ImageViews())
  {
    vk::FramebufferCreator framebuffer_creator{ device_ };
    framebuffer_creator.AddAttachment(image_view);
    framebuffer_creator.SetRenderPass(render_pass_);
    framebuffer_creator.SetSize(width_, height_);
    swapchain_framebuffers_.push_back(framebuffer_creator.Create());
  }

  // Create command pool
  vk::CommandPoolCreator command_pool_creator{ device_ };
  // TODO: find a graphics queue family to which command buffers will be submitted
  command_pool_creator.SetQueueFamilyIndex(0); // Graphics queue family index
  command_pool_ = command_pool_creator.Create();

  // Allocate command buffer
  vk::CommandBufferAllocator command_buffer_allocator{ device_, command_pool_ };
  swapchain_command_buffers_ = command_buffer_allocator.AllocateBuffers(swapchain_framebuffers_.size());

  // Record drawing commands
  for (int i = 0; i < swapchain_command_buffers_.size(); i++)
  {
    auto& swapchain_command_buffer = swapchain_command_buffers_[i];
    const auto& swapchain_framebuffer = swapchain_framebuffers_[i];

    swapchain_command_buffer.Begin();

    swapchain_command_buffer.SetClearColor(0.f, 0.f, 0.f, 1.f);
    swapchain_command_buffer.SetRenderArea(0, 0, width_, height_);
    swapchain_command_buffer.SetFramebuffer(swapchain_framebuffer);
    swapchain_command_buffer.CmdBeginRenderPass(render_pass_);

    swapchain_command_buffer.CmdBindPipeline(pipeline_);

    swapchain_command_buffer.CmdDraw(3, 1);

    swapchain_command_buffer.CmdEndRenderPass();

    swapchain_command_buffer.End();
  }

  // Create semaphores
  vk::SemaphoreCreator semaphore_creator{ device_ };
  image_available_semaphore_ = semaphore_creator.Create();
  render_finished_semaphore_ = semaphore_creator.Create();
}

void Engine::DrawFrame()
{
}

void Engine::Cleanup()
{
  image_available_semaphore_.Destroy();
  render_finished_semaphore_.Destroy();

  command_pool_.Destroy();

  for (auto& swapchain_framebuffer : swapchain_framebuffers_)
    swapchain_framebuffer.Destroy();

  pipeline_.Destroy();

  pipeline_layout_.Destroy();

  render_pass_.Destroy();

  vertex_shader_.Destroy();
  fragment_shader_.Destroy();

  buffer_.Destroy();
  device_memory_.Free();

  swapchain_.Destroy();

  device_.Destroy();

  surface_.Destroy();

  instance_.Destroy();

  glfwDestroyWindow(window_);
  glfwTerminate();
}
}
