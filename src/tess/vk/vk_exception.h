#ifndef TESS_VK_VK_EXCEPTION_H_
#define TESS_VK_VK_EXCEPTION_H_

#include <vulkan/vulkan.hpp>

namespace tess
{
namespace vk
{
class Exception : public std::exception
{
public:
  Exception() = delete;

  Exception(const std::string& str, VkResult result)
    : str_(str), result_(result)
  {
  }

  ~Exception() = default;

  std::string What()
  {
    return str_ + " (error enum: " + std::to_string(result_) + ")";
  }

  auto Result() const
  {
    return result_;
  }

private:
  std::string str_;
  VkResult result_;
};
}
}

#endif // TESS_VK_VK_EXCEPTION_H_
