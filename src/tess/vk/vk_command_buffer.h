#ifndef TESS_VK_VK_COMMAND_BUFFER_H_
#define TESS_VK_VK_COMMAND_BUFFER_H_

#include <array>

#include <vulkan/vulkan.h>

#include "tess/vk/vk_device.h"
#include "tess/vk/vk_render_pass.h"
#include "tess/vk/vk_framebuffer.h"
#include "tess/vk/vk_pipeline.h"

namespace tess
{
namespace vk
{
class CommandPool;
class CommandBuffer;

class CommandPoolCreator
{
public:
  CommandPoolCreator() = delete;

  explicit CommandPoolCreator(Device device);

  ~CommandPoolCreator();

  void SetQueueFamilyIndex(uint32_t queue_family_index);

  CommandPool Create();

private:
  VkCommandPoolCreateInfo create_info_{};

  Device device_;
};

class CommandPool
{
  friend class CommandPoolCreator;

public:
  CommandPool();

  explicit CommandPool(Device device);

  ~CommandPool();

  void Reset();
  void Destroy();

  operator VkCommandPool() const { return command_pool_; }

private:
  Device device_;

  VkCommandPool command_pool_ = NULL;
};

class CommandBufferAllocator
{
public:
  CommandBufferAllocator() = delete;

  CommandBufferAllocator(Device device, CommandPool command_pool);

  ~CommandBufferAllocator();

  std::vector<CommandBuffer> AllocateBuffers(int num_command_buffers);

private:
  Device device_;

  VkCommandBufferAllocateInfo allocate_info_{};
};

class CommandBuffer
{
  friend class CommandBufferAllocator;

public:
  CommandBuffer();

  ~CommandBuffer();

  // Command buffers will be automatically freed when their command pool is destroyed, so we don't need an explicit cleanup.

  void Begin();

  void SetFramebuffer(Framebuffer framebuffer);
  void SetRenderArea(int x, int y, int width, int height);
  void SetClearColor(float r, float g, float b, float a);
  void CmdBeginRenderPass(RenderPass render_pass);

  void CmdBindPipeline(Pipeline pipeline);

  void CmdDraw(int vertex_count, int instance_count, int first_vertex = 0, int first_instance = 0);

  void CmdEndRenderPass();

  void End();

private:
  Device device_;

  VkCommandBufferBeginInfo begin_info_{};

  VkClearValue clear_color_{};
  VkRenderPassBeginInfo render_pass_info_{};

  VkCommandBuffer command_buffer_ = NULL;
};
}
}

#endif // TESS_VK_VK_COMMAND_BUFFER_H_
