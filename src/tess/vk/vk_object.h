#ifndef TESS_VK_VK_OBJECT_H_
#define TESS_VK_VK_OBJECT_H_

#include <memory>
#include <vector>

namespace tess
{
namespace vk
{
class Object
{
public:
  Object();
  virtual ~Object();

  void AddDependency(Object* dependency);
  void Cleanup();

protected:
  virtual void Destroy();

private:
  std::vector<Object*> dependencies_;
};
}
}

#endif // TESS_VK_VK_OBJECT_H_
