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
    what_with_error_code_ = what_ + " (VkResult code: " + std::to_string(static_cast<int>(result_)) + ")";
  }

  const char* what() const override
  {
    return what_with_error_code_.c_str();
  }

  auto Result() const
  {
    return result_;
  }

private:
  std::string what_;
  VkResult result_ = VK_SUCCESS;

  std::string what_with_error_code_;
};
}
}

#endif // TESS_VK_VK_INSTANCE_H_
