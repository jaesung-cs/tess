#include "tess/vk/vk_object.h"

namespace tess
{
namespace vk
{
Object::Object()
{
}

Object::~Object()
{
  Cleanup();
}

void Object::Cleanup()
{
  for (auto* object : dependencies_)
  {
    if (object)
      object->Cleanup();
  }

  Destroy();

  dependencies_.clear();
}

void Object::AddDependency(Object* dependency)
{
  dependencies_.push_back(dependency);
}

void Object::Destroy()
{
}
}
}
