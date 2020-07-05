#include "tess/vk/vk_command_buffer.h"

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
//
// CommandPoolCreator
//
CommandPoolCreator::CommandPoolCreator(Device device)
  : device_(device)
{
  create_info_.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
}

CommandPoolCreator::~CommandPoolCreator()
{
}

void CommandPoolCreator::SetQueueFamilyIndex(uint32_t queue_family_index)
{
  create_info_.queueFamilyIndex = queue_family_index;
}

CommandPool CommandPoolCreator::Create()
{
  CommandPool command_pool{ device_ };

  if (VkResult result = vkCreateCommandPool(device_, &create_info_, NULL, &command_pool.command_pool_))
    throw Exception("Failed to create a command pool!", result);

  return command_pool;
}


//
// CommandPool
//
CommandPool::CommandPool() = default;

CommandPool::CommandPool(Device device)
  : device_(device)
{
}

CommandPool::~CommandPool()
{
}

void CommandPool::Reset()
{
}

void CommandPool::Destroy()
{
  if (command_pool_)
  {
    vkDestroyCommandPool(device_, command_pool_, NULL);
    command_pool_ = NULL;
  }
}


//
// CommandBufferAllocator
//
CommandBufferAllocator::CommandBufferAllocator(Device device, CommandPool command_pool)
  : device_(device)
{
  allocate_info_.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocate_info_.commandPool = command_pool;
  allocate_info_.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
}

CommandBufferAllocator::~CommandBufferAllocator()
{
}

std::vector<CommandBuffer> CommandBufferAllocator::AllocateBuffers(int num_command_buffers)
{
  std::vector<CommandBuffer> command_buffers;

  std::vector<VkCommandBuffer> command_buffer_handles(num_command_buffers);

  allocate_info_.commandBufferCount = num_command_buffers;

  if (VkResult result = vkAllocateCommandBuffers(device_, &allocate_info_, command_buffer_handles.data()))
    throw Exception("Failed to allocate " + std::to_string(num_command_buffers) + " command buffers!", result);

  for (const auto& command_buffer_handle : command_buffer_handles)
  {
    command_buffers.emplace_back();
    command_buffers.back().command_buffer_ = command_buffer_handle;
  }

  return command_buffers;
}


//
// CommandBuffer
//
CommandBuffer::CommandBuffer()
{
  begin_info_.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  render_pass_info_.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

  SetClearColor(0.f, 0.f, 0.f, 1.f);
}

CommandBuffer::~CommandBuffer()
{
}

void CommandBuffer::Begin()
{
  if (VkResult result = vkBeginCommandBuffer(command_buffer_, &begin_info_))
    throw Exception("Failed to begin recording command buffer!", result);
}

void CommandBuffer::SetFramebuffer(Framebuffer framebuffer)
{
  render_pass_info_.framebuffer = framebuffer;
}

void CommandBuffer::SetRenderArea(int x, int y, int width, int height)
{
  render_pass_info_.renderArea.offset.x = x;
  render_pass_info_.renderArea.offset.y = y;
  render_pass_info_.renderArea.extent.width = width;
  render_pass_info_.renderArea.extent.height = height;
}

void CommandBuffer::SetClearColor(float r, float g, float b, float a)
{
  render_pass_info_.clearValueCount = 1;
  render_pass_info_.pClearValues = &clear_color_;

  clear_color_.color.float32[0] = r;
  clear_color_.color.float32[1] = g;
  clear_color_.color.float32[2] = b;
  clear_color_.color.float32[3] = a;
}

void CommandBuffer::CmdBeginRenderPass(RenderPass render_pass)
{
  render_pass_info_.renderPass = render_pass;

  vkCmdBeginRenderPass(command_buffer_, &render_pass_info_, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::CmdBindPipeline(Pipeline pipeline)
{
  vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void CommandBuffer::CmdDraw(int vertex_count, int instance_count, int first_vertex, int first_instance)
{
  vkCmdDraw(command_buffer_, vertex_count, instance_count, first_vertex, first_instance);
}

void CommandBuffer::CmdEndRenderPass()
{
  vkCmdEndRenderPass(command_buffer_);
}

void CommandBuffer::End()
{
  if (VkResult result = vkEndCommandBuffer(command_buffer_))
    throw Exception("Failed to record command buffer!", result);
}
}
}
