#include "shader.h"

ShaderUPtr Shader::CreateFromFile(const std::string& filename,
  GLenum shaderType) {
  auto shader = std::unique_ptr<Shader>(new Shader());
  if (!shader->LoadFile(filename, shaderType))
    return nullptr;
  return std::move(shader);
}

Shader::~Shader() {
    if (m_shader){
        glDeleteShader(m_shader);
    }
}

bool Shader::LoadFile(const std::string& filename, GLenum shaderType) {
  auto result = LoadTextFile(filename);
  if (!result.has_value())
    return false;

  auto& code = result.value(); // the size of txt result file can be very huge. If I declare it without ampersand, the whole memory will be copied--> unncessary memory 
                                // auto cannot decide if & is necessary or not
  const char* codePtr = code.c_str(); // declare c style string prt
  int32_t codeLength = (int32_t)code.length();


  //
  m_shader = glCreateShader(shaderType); // handle number is stored into m_shader

  //second parameter: the number of codes, thrid: code pointer, fourth: code length
  glShaderSource(m_shader, 1, (const GLchar* const*)&codePtr, &codeLength);
  glCompileShader(m_shader);


    // check compile error
  int success = 0;
  glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success); // get shader status
  if (!success) {
    char infoLog[1024];
    glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);
    SPDLOG_ERROR("failed to compile shader: \"{}\"", filename);
    SPDLOG_ERROR("reason: {}", infoLog);
    return false;
  }
  return true;
}

