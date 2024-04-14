#pragma once
#include <vector>
#include <string>
#include <iostream>
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
	void addTextureToTextureArray(std::string texture, int texture_id);
	//void loadTexture(int sampler_location);
	void addCubemap(std::vector<std::string> texture_faces, GLuint& texture_id);
	void addSingleTexture(std::string path, GLuint& handle);
	void generateMipmap();
	GLuint getSkyboxTextureId();
	GLuint getWaterTextureId();
	GLuint getTextureArrayId();
	GLuint getCloudNoiseId();

private:
	int m_texture_width;
	int m_texture_height;
	int m_textures_max_amount;
	int m_stb_nr_channels{ 0 };
	GLuint m_texture_array{ 0 };
	GLuint m_skybox_id;
	GLuint m_water_id;
	GLuint m_cloud_noise_id;
	std::vector<std::string> m_textures{};
};

