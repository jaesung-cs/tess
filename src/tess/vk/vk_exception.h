#ifndef TESS_VK_VK_EXCEPTION_H_
#define TESS_VK_VK_EXCEPTION_H_

#include <stdexcept>
#include <string>

#include <vulkan/vulkan.h>

namespace tess
{
namespace vk
{
class Exception : public std::exception
{
public:
  Exception() = delete;

  explicit Exception(VkResult result)
    : result_(result)
  {
  }

  Exception(const std::string& what, VkResult result)
    : what_(what), result_(result)
  {
  }

  const char* what() const override
  {
    return (what_ + " (VkResult code: " + std::to_string(static_cast<int>(result_)) + ")").c_str();
  }

private:
  std::string what_;
  VkResult result_ = VK_SUCCESS;
};
}
}

#endif // TESS_VK_VK_INSTANCE_H_
