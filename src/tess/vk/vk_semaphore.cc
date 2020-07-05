#include "tess/vk/vk_semaphore.h"

#include "tess/vk/vk_exception.h"

namespace tess
{
namespace vk
{
//
// SemaphoreCreator
//
SemaphoreCreator::SemaphoreCreator(Device device)
  : device_(device)
{
  create_info_.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
}

SemaphoreCreator::~SemaphoreCreator()
{
}

Semaphore SemaphoreCreator::Create()
{
  Semaphore semaphore{ device_ };

  if (VkResult result = vkCreateSemaphore(device_, &create_info_, NULL, &semaphore.semaphore_))
    throw Exception("Failed to create a semaphore!", result);

  return semaphore;
}

//
// Semaphore
//
Semaphore::Semaphore()
{
}

Semaphore::Semaphore(Device device)
  : device_(device)
{
}

Semaphore::~Semaphore()
{
}

void Semaphore::Destroy()
{
  if (semaphore_)
  {
    vkDestroySemaphore(device_, semaphore_, NULL);
    semaphore_ = NULL;
  }
}
}
}
