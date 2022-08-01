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

	std::vector<std::string> textures{};
	void generateMipmap();
	void addTexture(std::string item);
	std::string loadTexture(std::string path);

};

