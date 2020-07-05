#include "tess/vk/vk_buffer.h"

#include "tess/vk/vk_device_memory.h"
#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
//
// DeviceMemoryAllocator
//
BufferCreator::BufferCreator(Device device)
  : device_(device)
{
  create_info_.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info_.pNext = NULL;
  create_info_.flags = 0;
  create_info_.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  UseAsVertexBuffer();
}

BufferCreator::~BufferCreator()
{
}

void BufferCreator::UseAsVertexBuffer()
{
  create_info_.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
}

Buffer BufferCreator::Create(int bytes)
{
  create_info_.size = bytes;

  Buffer buffer{ device_ };

  if (VkResult result = vkCreateBuffer(device_, &create_info_, NULL, &buffer.buffer_))
    throw Exception("Failed to create vertex buffer!", result);

  buffer.LoadMemoryRequirements();

  return buffer;
}


//
// Buffer
//
Buffer::Buffer()
{
}

Buffer::Buffer(Device device)
  : device_(device)
{
}

Buffer::~Buffer()
{
}

void Buffer::Destroy()
{
  if (buffer_)
  {
    vkDestroyBuffer(device_, buffer_, NULL);
    buffer_ = NULL;
  }
}

void Buffer::LoadMemoryRequirements()
{
  vkGetBufferMemoryRequirements(device_, buffer_, &memory_requirements_);
}

void Buffer::BindMemory(DeviceMemory memory, uint64_t offset)
{
  vkBindBufferMemory(device_, buffer_, memory, offset);
}
}
}
