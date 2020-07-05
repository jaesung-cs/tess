#include "tess/vk/vk_fence.h"

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
//
// FenceCreator
//
FenceCreator::FenceCreator(Device device)
  : device_(device)
{
  create_info_.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  create_info_.flags = VK_FENCE_CREATE_SIGNALED_BIT;
}

FenceCreator::~FenceCreator()
{
}

Fence FenceCreator::Create()
{
  Fence fence{ device_ };

  if (VkResult result = vkCreateFence(device_, &create_info_, NULL, &fence.fence_))
    throw Exception("Failed to create a fence!", result);

  return fence;
}

//
// Fence
//
Fence::Fence()
{
}

Fence::Fence(Device device)
  : device_(device)
{
}

Fence::~Fence()
{
}

void Fence::Destroy()
{
  if (fence_ != VK_NULL_HANDLE)
  {
    vkDestroyFence(device_, fence_, NULL);
    fence_ = VK_NULL_HANDLE;
  }
}

bool Fence::IsValid()
{
  return fence_ != VK_NULL_HANDLE;
}

void Fence::Wait()
{
  vkWaitForFences(device_, 1, &fence_, VK_TRUE, UINT64_MAX);
}

void Fence::Reset()
{
  vkResetFences(device_, 1, &fence_);
}
}
}
