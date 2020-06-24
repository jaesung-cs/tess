#ifndef TESS_ENGINE_H_
#define TESS_ENGINE_H_

#include <memory>
#include <vector>
#include <optional>

#include <vulkan/vulkan.hpp>

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

  const std::vector<const char*> validation_layers_ =
  {
    "VK_LAYER_KHRONOS_validation"
  };

  const std::vector<const char*> device_extensions_ =
  {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };

#ifdef NDEBUG
  const bool enable_validation_layers_ = false;
#else
  const bool enable_validation_layers_ = true;
#endif

  bool CheckValidationLayerSupport();

  void Initialize();
  void InitializeGlfw();
  void InitializeVulkan();

  void Cleanup();

  void CheckVulkanExtensionSupport();

  void CreateInstance();

  std::vector<const char*> GetRequiredExtensions();

  void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info);
  void SetupDebugMessenger();

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

  void PickPhysicalDevice();

  void CreateLogicalDevice();

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
  VkInstance instance_ = nullptr;
  VkDebugUtilsMessengerEXT debug_messenger_ = nullptr;
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice device_ = nullptr;
  VkQueue graphics_queue_ = nullptr;
  VkQueue present_queue_ = nullptr;
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
};
}

#endif // TESS_ENGINE_H_
