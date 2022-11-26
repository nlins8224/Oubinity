#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <glad/glad.h>
#include "stb_image.h"
#include "Shader.h"
#include <optick.h>
class TextureManager
{
public:
	TextureManager(Shader& shader, int texture_witdh, int texture_height, int textures_max_amount);
	void addTexture(std::string texture, int texture_id);
	void loadTexture();
	void generateMipmap();

private:
	int m_texture_width;
	int m_texture_height;
	int m_textures_max_amount;
	int m_stb_nr_channels{ 0 };
	GLuint m_texture_array{ 0 };
	Shader& m_shader;

	std::vector<std::string> m_textures{};
};

