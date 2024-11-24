#pragma once

#include <glad/glad.h>

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>

typedef struct {
  std::string vertex;
  std::string fragment;
} shader_code;

class Shader {
 public:
  Shader(std::string vertex_path, std::string fragment_path);
  unsigned int getID() const;
  void setUniformBool(const std::string& name, bool value) const;
  void setUniformInt(const std::string& name, int value) const;
  void setUniformFloat(const std::string& name, float value) const;
  void setUniformMat4(const std::string& name, const glm::mat4& mat) const;
  void setUniformVec3f(const std::string& name, const glm::vec3& vec) const;
  void setUniformVec3i(const std::string& name, const glm::ivec3& vec) const;
  void setUniformVec2i(const std::string& name, const glm::ivec2& vec) const;
  void setUniformVec2f(const std::string& name, const glm::vec2& vec) const;
  GLint getUniformLocation(const std::string& name) const;
  void bind() const;

 private:
  unsigned int m_id;

  shader_code parseShader(std::string vertex_path, std::string fragment_path);
  unsigned int compileShader(int type, const std::string code);
  void createShaderProgram(unsigned int vertex_shader,
                           unsigned int fragment_shader);
  void checkCompileErrors(unsigned int id, int program_type) const;
  void unbind() const;
};