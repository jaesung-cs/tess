#include "tess/vk/vk_queue.h"

#include "tess/vk/vk_semaphore.h"
#include "tess/vk/vk_command_buffer.h"
#include "tess/vk/vk_swapchain.h"
#include "tess/vk/vk_fence.h"
#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
Queue::Queue()
{
  // Submit
  submit_info_.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  // Present
  present_info_.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
}

Queue::~Queue()
{
}

void Queue::AddSubmitColorAttachmentOutputWaitStage(Semaphore semaphore)
{
  wait_stages_.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
  wait_semaphores_.push_back(semaphore);
}

void Queue::AddSubmitSignalSemaphores(Semaphore semaphore)
{
  signal_semaphores_.push_back(semaphore);
}

void Queue::AddSubmitCommandBuffer(CommandBuffer command_buffer)
{
  command_buffers_.push_back(command_buffer);
}

void Queue::Submit()
{
  submit_info_.waitSemaphoreCount = wait_semaphores_.size();
  submit_info_.pWaitSemaphores = wait_semaphores_.data();
  submit_info_.pWaitDstStageMask = wait_stages_.data();

  submit_info_.signalSemaphoreCount = signal_semaphores_.size();
  submit_info_.pSignalSemaphores = signal_semaphores_.data();

  submit_info_.commandBufferCount = command_buffers_.size();
  submit_info_.pCommandBuffers = command_buffers_.data();

  VkResult result = vkQueueSubmit(queue_, 1, &submit_info_, VK_NULL_HANDLE);

  // Clear intermediate data
  wait_semaphores_.clear();
  wait_stages_.clear();
  signal_semaphores_.clear();
  command_buffers_.clear();

  if (result)
    throw Exception("Failed to submit command buffer!", result);
}

void Queue::Submit(Fence fence)
{
  // TODO: handle with/without fence cases in one place
  submit_info_.waitSemaphoreCount = wait_semaphores_.size();
  submit_info_.pWaitSemaphores = wait_semaphores_.data();
  submit_info_.pWaitDstStageMask = wait_stages_.data();

  submit_info_.signalSemaphoreCount = signal_semaphores_.size();
  submit_info_.pSignalSemaphores = signal_semaphores_.data();

  submit_info_.commandBufferCount = command_buffers_.size();
  submit_info_.pCommandBuffers = command_buffers_.data();

  VkResult result = vkQueueSubmit(queue_, 1, &submit_info_, fence);

  // Clear intermediate data
  wait_semaphores_.clear();
  wait_stages_.clear();
  signal_semaphores_.clear();
  command_buffers_.clear();

  if (result)
    throw Exception("Failed to submit command buffer!", result);
}

void Queue::AddPresentWaitSemaphore(Semaphore semaphore)
{
  wait_semaphores_.push_back(semaphore);
}

void Queue::SetPresentSwapchain(Swapchain swapchain)
{
  swapchain_ = swapchain;
}

void Queue::SetPresentImageIndex(int image_index)
{
  image_index_ = image_index;
}

void Queue::Present()
{
  present_info_.waitSemaphoreCount = wait_semaphores_.size();
  present_info_.pWaitSemaphores = wait_semaphores_.data();

  present_info_.swapchainCount = 1;
  present_info_.pSwapchains = &swapchain_;
  present_info_.pImageIndices = &image_index_;

  VkResult result = vkQueuePresentKHR(queue_, &present_info_);

  // Clear intermediate data
  wait_semaphores_.clear();

  if (result)
    throw Exception("Failed to present!", result);
}

void Queue::WaitIdle()
{
  vkQueueWaitIdle(queue_);
}
}
}
