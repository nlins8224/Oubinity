#include "TextureManager.h"

TextureManager::TextureManager(int texture_width, int texture_height, int textures_max_amount) 
	: 
	m_texture_width{texture_width},
	m_texture_height{texture_height},
	m_textures_max_amount{textures_max_amount}
{
	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	std::string texture_data = loadTexture("");

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
	auto texture_image = stbi_load();
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_image);
	auto itr = std::find(textures.begin(), textures.end(), texture);
	int texture_index = std::distance(textures.begin(), itr);
	// insert texture to texture array
	glTexSubImage3D(
		GL_TEXTURE_2D_ARRAY,
		0,
		0,
		texture_index,
		m_texture_width,
		m_texture_height,
		1,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		texture_image // TODO: load image
		
}

std::string TextureManager::loadTexture(std::string path)
{
	int width, height, nrChannels;
	return stbi_load(path, &width, &height, &nrChannels, 0);
}


