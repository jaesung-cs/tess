#include "tess/gl/gl_shader.h"

namespace tess
{
namespace gl
{
void Shader::Load(const std::string& filename)
{
  static const std::unordered_map<std::string, Type> extension_to_type =
  {
    {".vert", Type::VERTEX},
    {".tese", Type::TESS_EVALUATION},
    {".tesc", Type::TESS_CONTROL},
    {".geom", Type::GEOMETRY},
    {".frag", Type::FRAGMENT},
    {".comp", Type::COMPUTE},
  };

  static const std::unordered_map<Type, GLenum> type_to_gl_enum =
  {
    {Type::VERTEX, GL_VERTEX_SHADER},
    {Type::TESS_CONTROL, GL_TESS_CONTROL_SHADER},
    {Type::TESS_EVALUATION, GL_TESS_EVALUATION_SHADER},
    {Type::GEOMETRY, GL_GEOMETRY_SHADER},
    {Type::FRAGMENT, GL_FRAGMENT_SHADER},
    {Type::COMPUTE, GL_COMPUTE_SHADER},
  };

  if (filename.length() < 6)
    return;

  std::ifstream shader_file;

  // Ensure ifstream objects can throw exceptions:
  shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try
  {
    // Open files
    shader_file.open(filename);

    // Read file's buffer contents into streams
    std::stringstream shader_stream;
    shader_stream << shader_file.rdbuf();

    // Convert stream into string
    code_ = shader_stream.str();

    // Close file handlers
    shader_file.close();

    if (id_)
      glDeleteShader(id_);

    auto extension = filename.substr(filename.length() - 5);
    auto type = extension_to_type.find(extension)->second;
    auto gl_enum = type_to_gl_enum.find(type)->second;

    id_ = glCreateShader(gl_enum);
  }
  catch (std::ifstream::failure e)
  {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;

    if (id_)
      glDeleteShader(id_);
  }
}

void Shader::Compile()
{
  auto code_ptr = code_.c_str();

  glShaderSource(id_, 1, &code_ptr, NULL);
  glCompileShader(id_);

  // Check compile errors
  int success = 0;
  char info_log[512];

  glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    // Print compile errors if any
    glGetShaderInfoLog(id_, 512, NULL, info_log);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;

    glDeleteShader(id_);
    id_ = 0;
  }
}
}
}
