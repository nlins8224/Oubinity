#pragma once
#include <vector>
#include <string>
#include <glad/glad.h>
#include "stb_image.h"
class TextureManager
{
public:
	TextureManager(int texture_witdh, int texture_height, int textures_max_amount);
	
private:
	int m_texture_width;
	int m_texture_height;
	int m_textures_max_amount;
	int m_stb_nr_channels{ 0 };
	GLuint m_texture_array{ 0 };

	std::vector<std::string> m_textures{};
	void generateMipmap();
	void addTexture(std::string texture);
	std::string loadTexture(std::string path);
	int getTextureIndex(std::string texture);
};

