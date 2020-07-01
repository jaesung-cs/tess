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

  const std::vector<const char*> device_extensions_ =
  {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };

  void Initialize();
  void InitializeGlfw();
  void InitializeVulkan();

  void Cleanup();

  std::vector<const char*> GetRequiredExtensions();

  struct QueueFamilyIndices
  {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool IsComplete()
    {
      return graphics_family.has_value() && present_family.has_value();
    }
  };

  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
  int IsDeviceSuitable(VkPhysicalDevice device);

  void CreateSurface();

  bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

  struct SwapChainSupportDetails
  {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
  };

  SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
  VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);

  void CreateSwapChain();
  void CleanupSwapChain();
  void RecreateSwapChain();

  void CreateImageViews();

  void CreateGraphicsPipeline();

  VkShaderModule CreateShaderModule(const std::vector<char>& code);

  void CreateRenderPass();

  void CreateFramebuffers();

  void CreateCommandPool();

  void CreateCommandBuffers();

  void DrawFrame();

  void CreateSyncObjects();

  GLFWwindow* window_ = nullptr;
  VkSurfaceKHR surface_ = nullptr;
  VkSwapchainKHR swap_chain_ = nullptr;
  std::vector<VkImage> swap_chain_images_;
  VkFormat swap_chain_image_format_ = VK_FORMAT_UNDEFINED;
  VkExtent2D swap_chain_extent_{ 0, 0 };
  std::vector<VkImageView> swap_chain_image_views_;
  VkShaderModule vert_shader_module_ = nullptr;
  VkShaderModule frag_shader_module_ = nullptr;
  VkRenderPass render_pass_ = nullptr;
  VkPipelineLayout pipeline_layout_ = nullptr;
  VkPipeline graphics_pipeline_ = nullptr;
  std::vector<VkFramebuffer> swap_chain_framebuffers_;
  VkCommandPool command_pool_ = nullptr;
  std::vector<VkCommandBuffer> command_buffers_;
  std::vector<VkSemaphore> image_available_semaphores_;
  std::vector<VkSemaphore> render_finished_semaphores_;
  std::vector<VkFence> in_flight_fences_;
  std::vector<VkFence> images_in_flight_;
  size_t current_frame_ = 0;

  bool framebuffer_resized_ = false;

  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  struct Vertex
  {
    float pos[2];
    float color[3];
  };

  // Vulkan wrapper
  vk::LayerExtension layer_extension_;
  vk::Instance instance_;
  vk::Device device_;
  vk::Queue graphics_queue_;
  vk::Queue present_queue_;
};
}

#endif // TESS_ENGINE_H_
