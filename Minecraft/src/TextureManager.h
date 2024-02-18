#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <optick.h>
#include <glad/glad.h>
#include "block/Block.h"
#include "third_party/stb_image.h"
#include "shader/Shader.h"
#include "loguru.hpp"

class TextureManager
{
public:
	TextureManager(int texture_witdh, int texture_height, int textures_max_amount);
	void addTextures();
	void addTexture(std::string texture, int texture_id);
	//void loadTexture(int sampler_location);
	void addCubemap(std::vector<std::string> texture_faces, GLuint& texture_id);
	void addWater(std::string path);
	void generateMipmap();
	GLuint getSkyboxTextureId();
	GLuint getWaterTextureId();
	GLuint getTextureArrayId();

private:
	int m_texture_width;
	int m_texture_height;
	int m_textures_max_amount;
	int m_stb_nr_channels{ 0 };
	GLuint m_texture_array{ 0 };
	GLuint m_skybox_id;
	GLuint m_water_id;
	std::vector<std::string> m_textures{};
};

