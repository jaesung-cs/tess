#ifndef TESS_SHADER_H_
#define TESS_SHADER_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>

namespace tess
{
class Shader
{
public:
  enum class Type
  {
    VERTEX,
    TESS_CONTROL,
    TESS_EVALUATION,
    GEOMETRY,
    FRAGMENT,
    COMPUTE,
    UNDEFINED
  };

public:
  Shader() = delete;

  explicit Shader(const std::string& filename)
  {
    Load(filename);

    if (id_)
      Compile();
  }

  ~Shader()
  {
    if (id_)
      glDeleteShader(id_);
  }

  auto Id() const { return id_; }

private:
  void Load(const std::string& filename);

  void Compile();

  GLuint id_ = 0;

  Type type_ = Type::UNDEFINED;
  std::string code_;
};
}

#endif // TESS_SHADER_H_
