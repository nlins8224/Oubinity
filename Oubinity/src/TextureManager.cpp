#include "TextureManager.h"

namespace Skybox {
std::vector<std::string> skybox_faces = {"right",  "left",  "top",
                                         "bottom", "front", "back"};
}

TextureManager::TextureManager(int texture_width, int texture_height,
                               int textures_max_amount)
    : m_texture_width{texture_width},
      m_texture_height{texture_height},
      m_textures_max_amount{textures_max_amount}

{
  addSingleTexture("textures/water.png", m_water_id);
  addSingleTexture("textures/sky/noise5.png", m_cloud_noise_id);
  addTextures();
  // has to be after addTextures
  addCubemap(Skybox::skybox_faces, m_skybox_id);
}

void TextureManager::generateMipmap() { glGenerateMipmap(GL_TEXTURE_2D_ARRAY); }

void TextureManager::addTextures() {
  glGenTextures(1, &m_texture_array);
  glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_array);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // create texture array
  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, m_texture_width,
               m_texture_height, m_textures_max_amount, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);

  std::string texture_name;
  for (int i = Block::DIRT; i != Block::AMOUNT; i++) {
    Block::block_id id = static_cast<Block::block_id>(i);
    texture_name = Block::getBlockType(id).texture;
    addTextureToTextureArray(texture_name, id);
  }
}

void TextureManager::addTextureToTextureArray(std::string texture,
                                              int texture_id) {
  const bool is_in = std::find(m_textures.begin(), m_textures.end(), texture) !=
                     m_textures.end();
  if (is_in) return;

  m_textures.push_back(texture);
  std::string path = "textures/" + texture + ".png";
  int width, height, channels;
  unsigned char* texture_image =
      stbi_load(path.c_str(), &width, &height, &channels, 0);
  if (!texture_image) {
    LOG_F(ERROR, "Failed to add texture under path: '%s'", path);
  }
  glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_array);
  // insert texture to texture array
  glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, texture_id, m_texture_width,
                  m_texture_height, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                  texture_image);
  stbi_image_free(texture_image);
}

// void TextureManager::loadTexture(int sampler_location)
//{
//	const std::string& texture_array_sampler = "texture_array_sampler";
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_array);
//	glUniform1i(sampler_location, 0);
//}

void TextureManager::addSingleTexture(std::string path, GLuint& handle) {
  glGenTextures(1, &handle);
  glBindTexture(GL_TEXTURE_2D, handle);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                  GL_REPEAT);  // set texture wrapping to GL_REPEAT (default
                               // wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenerateMipmap(GL_TEXTURE_2D);

  int width, height, nrChannels;
  unsigned char* data =
      stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
  if (data) {
    GLenum format;
    switch (nrChannels) {
      case 1:
        format = GL_LUMINANCE;
        break;
      case 2:
        format = GL_LUMINANCE_ALPHA;
        break;
      case 3:
        format = GL_RGB;
        break;
      case 4:
        format = GL_RGBA;
        break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    LOG_F(ERROR, "Failed to add texture under path: '%s'", path);
  }
  stbi_image_free(data);
}

void TextureManager::addCubemap(std::vector<std::string> texture_faces,
                                GLuint& texture_id) {
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

  unsigned char* data;
  for (unsigned int i = 0; i < texture_faces.size(); i++) {
    std::string path = "textures/skybox/" + texture_faces[i] + ".png";
    data = stbi_load(path.c_str(), &m_texture_width, &m_texture_height,
                     &m_stb_nr_channels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                   m_texture_width, m_texture_height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data);
      LOG_F(INFO, "Texture loaded, path: '%s'", path);
    } else {
      LOG_F(ERROR, "Cubemap tex failed to load at path: '%s'", path);
      stbi_image_free(data);
    }
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

GLuint TextureManager::getSkyboxTextureId() { return m_skybox_id; }

GLuint TextureManager::getWaterTextureId() { return m_water_id; }

GLuint TextureManager::getTextureArrayId() { return m_texture_array; }

GLuint TextureManager::getCloudNoiseId() { return m_cloud_noise_id; }
