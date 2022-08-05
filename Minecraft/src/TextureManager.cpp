#include "TextureManager.h"

TextureManager::TextureManager(Shader& shader, int texture_width, int texture_height, int textures_max_amount) 
	: 
	m_shader{shader},
	m_texture_width{texture_width},
	m_texture_height{texture_height},
	m_textures_max_amount{textures_max_amount}
{
	glGenTextures(1, &m_texture_array);
	glBindTexture(GL_TEXTURE_2D, m_texture_array);

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
	const bool is_in = std::find(m_textures.begin(), m_textures.end(), texture) != m_textures.end();
	if (!is_in)
		m_textures.push_back(texture);
	std::string path = "textures/" + texture + ".png";
	unsigned char *texture_image = stbi_load(path.c_str(), &m_texture_width, &m_texture_height, &m_stb_nr_channels, 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_array);
	int texture_index = getTextureIndex(texture);
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

std::string TextureManager::loadTexture()
{
	return "";
}

int TextureManager::getTextureIndex(std::string texture)
{
	auto itr = std::find(m_textures.begin(), m_textures.end(), texture);
	return std::distance(m_textures.begin(), itr);
}
