#include "tess/gl/gl_program.h"

#include <unordered_map>
#include <filesystem>

namespace tess
{
namespace gl
{
namespace fs = std::filesystem;

Program::Program(std::vector<std::shared_ptr<Shader>> shaders)
{
  for (auto shader : shaders)
    AddShader(shader);

  Link();
}

Program::Program(const std::string& directory, const std::string& name)
{
  static std::vector<std::string> extensions =
  {
    ".vert",
    ".tese",
    ".tesc",
    ".geom",
    ".frag",
    ".comp",
  };

  for (const auto& extension : extensions)
  {
    auto filename = directory + '\\' + name + extension;
    if (fs::exists(filename))
      AddShader(std::make_shared<Shader>(filename));
  }

  Link();
}

Program::~Program()
{
  if (id_)
    glDeleteProgram(id_);
}

void Program::AddShader(std::shared_ptr<Shader> shader)
{
  shaders_.push_back(shader);
}

void Program::Link()
{
  id_ = glCreateProgram();

  for (auto shader : shaders_)
  {
    if (shader->Id())
      glAttachShader(id_, shader->Id());
  }

  glLinkProgram(id_);

  for (auto shader : shaders_)
  {
    if (shader->Id())
      glDetachShader(id_, shader->Id());
  }

  // Print linking errors if any
  int success = 0;
  char info_log[512];

  glGetProgramiv(id_, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(id_, 512, NULL, info_log);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;

    glDeleteProgram(id_);
  }
}

void Program::Use()
{
  glUseProgram(id_);
}

void Program::Uniform1f(const std::string name, float v)
{
  glProgramUniform1f(id_, glGetUniformLocation(id_, name.c_str()), v);
}

void Program::Uniform2f(const std::string name, float v0, float v1)
{
  glProgramUniform2f(id_, glGetUniformLocation(id_, name.c_str()), v0, v1);
}

void Program::Uniform2f(const std::string name, const Vector2f& v)
{
  glProgramUniform2fv(id_, glGetUniformLocation(id_, name.c_str()), 1, v.data());
}

void Program::Uniform2i(const std::string name, int v0, int v1)
{
  glProgramUniform2i(id_, glGetUniformLocation(id_, name.c_str()), v0, v1);
}

void Program::Uniform2i(const std::string name, const Vector2i& v)
{
  glProgramUniform2iv(id_, glGetUniformLocation(id_, name.c_str()), 1, v.data());
}

void Program::Uniform3f(const std::string name, float v0, float v1, float v2)
{
  glProgramUniform3f(id_, glGetUniformLocation(id_, name.c_str()), v0, v1, v2);
}

void Program::Uniform3f(const std::string name, const Vector3f& v)
{
  glProgramUniform3fv(id_, glGetUniformLocation(id_, name.c_str()), 1, v.data());
}

void Program::Uniform4f(const std::string name, float v0, float v1, float v2, float v3)
{
  glProgramUniform4f(id_, glGetUniformLocation(id_, name.c_str()), v0, v1, v2, v3);
}

void Program::Uniform4f(const std::string name, const Vector4f& v)
{
  glProgramUniform4fv(id_, glGetUniformLocation(id_, name.c_str()), 1, v.data());
}

void Program::Uniform4i(const std::string name, int v0, int v1, int v2, int v3)
{
  glProgramUniform4i(id_, glGetUniformLocation(id_, name.c_str()), v0, v1, v2, v3);
}

void Program::Uniform4i(const std::string name, const Vector4i& v)
{
  glProgramUniform4iv(id_, glGetUniformLocation(id_, name.c_str()), 1, v.data());
}

void Program::UniformMatrix3f(const std::string name, const Matrix3f& m)
{
  glProgramUniformMatrix3fv(id_, glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, m.data());
}

void Program::UniformMatrix4f(const std::string name, const Matrix4f& m)
{
  glProgramUniformMatrix4fv(id_, glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, m.data());
}
}
}
