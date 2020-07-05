#ifndef TESS_VK_VK_QUEUE_H_
#define TESS_VK_VK_QUEUE_H_

#include <vector>

#include <vulkan/vulkan.h>

namespace tess
{
namespace vk
{
class Semaphore;
class CommandBuffer;
class Swapchain;
class Fence;

class Queue
{
  friend class DeviceCreator;

public:
  Queue();
  ~Queue();

  operator VkQueue () { return queue_; }

  // Submit
  void AddSubmitColorAttachmentOutputWaitStage(Semaphore semaphore);
  void AddSubmitSignalSemaphores(Semaphore semaphore);
  void AddSubmitCommandBuffer(CommandBuffer command_buffer);
  void Submit();
  void Submit(Fence fence);

  // Present
  void AddPresentWaitSemaphore(Semaphore semaphore);
  void SetPresentSwapchain(Swapchain swapchain);
  void SetPresentImageIndex(int image_index);
  void Present();

  void WaitIdle();

private:
  VkSubmitInfo submit_info_{};

  // TODO: wait_semaphores_ is shared by both submit and present.
  std::vector<VkSemaphore> wait_semaphores_;
  std::vector<VkPipelineStageFlags> wait_stages_;

  std::vector<VkSemaphore> signal_semaphores_;

  std::vector<VkCommandBuffer> command_buffers_;

  // Present queue
  VkSwapchainKHR swapchain_ = NULL;
  uint32_t image_index_ = 0;
  VkPresentInfoKHR present_info_{};

  VkQueue queue_;
};
}
}

#endif // TESS_VK_VK_QUEUE_H_
