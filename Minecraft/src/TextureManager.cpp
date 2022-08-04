#include "TextureManager.h"

TextureManager::TextureManager(int texture_width, int texture_height, int textures_max_amount) 
	: 
	m_texture_width{texture_width},
	m_texture_height{texture_height},
	m_textures_max_amount{textures_max_amount}
{
	glGenTextures(1, &m_texture_array);
	glBindTexture(GL_TEXTURE_2D, m_texture_array);
	//std::string texture_data = loadTexture("");

	// create texture array
	glTexImage3D(
		GL_TEXTURE_2D_ARRAY,
		0,
		GL_RGBA,
		m_texture_width,
		m_texture_height,
		m_textures_max_amount,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		nullptr
	);
}

void TextureManager::generateMipmap()
{
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

void TextureManager::addTexture(std::string texture)
{
	const bool is_in = std::find(textures.begin(), textures.end(), texture) != textures.end();
	if (!is_in)
		textures.push_back(texture);
	// TODO: Load image
	std::string path = "textures/" + texture + ".png";
	unsigned char *texture_image = stbi_load(path.c_str(), &m_texture_width, &m_texture_height, &m_stb_nr_channels, 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_array);
	auto itr = std::find(textures.begin(), textures.end(), texture);
	int texture_index = std::distance(textures.begin(), itr);
	// insert texture to texture array
	glTexSubImage3D(
		GL_TEXTURE_2D_ARRAY,
		0,
		0,
		0,
		texture_index,
		m_texture_width,
		m_texture_height,
		1,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		texture_image
	);
	
}

std::string TextureManager::loadTexture(std::string path)
{
	return "";
}
