#ifndef TESS_GL_GL_PROGRAM_H_
#define TESS_GL_GL_PROGRAM_H_

#include <string>
#include <vector>

#include <glad/glad.h>

#include "tess/gl/gl_shader.h"
#include "tess/types.h"

namespace tess
{
namespace gl
{
class Program
{
public:
  Program();

  Program(std::vector<std::shared_ptr<Shader>> shaders);

  Program(const std::string& directory, const std::string& name);

  ~Program();

  operator GLuint () { return id_; }

  void AddShader(std::shared_ptr<Shader> shader);

  void Link();

  void Use();

  void Uniform1f(const std::string name, float v);
  void Uniform2f(const std::string name, float v0, float v1);
  void Uniform2f(const std::string name, const Vector2f& v);
  void Uniform2i(const std::string name, int v0, int v1);
  void Uniform2i(const std::string name, const Vector2i& v);
  void Uniform3f(const std::string name, float v0, float v1, float v2);
  void Uniform3f(const std::string name, const Vector3f& v);
  void Uniform4f(const std::string name, float v0, float v1, float v2, float v3);
  void Uniform4f(const std::string name, const Vector4f& v);
  void Uniform4i(const std::string name, int v0, int v1, int v2, int v3);
  void Uniform4i(const std::string name, const Vector4i& v);
  void UniformMatrix3f(const std::string name, const Matrix3f& m);
  void UniformMatrix4f(const std::string name, const Matrix4f& m);

private:
  GLuint id_ = 0;

  std::vector<std::shared_ptr<Shader>> shaders_;
};
}
}

#endif // TESS_GL_GL_PROGRAM_H_
