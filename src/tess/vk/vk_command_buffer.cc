#include "tess/vk/vk_command_buffer.h"

#include <iostream>

namespace tess
{
namespace vk
{
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
  vkDestroyCommandPool(device_, command_pool_, NULL);
}

CommandBuffer::CommandBuffer()
{
}

CommandBuffer::~CommandBuffer()
{
}
}
}
