#pragma once
#include "glad/glad.h"
#include <iostream>

class FrameBuffer
{
public:
	FrameBuffer(float width, float height);
	~FrameBuffer();
	void rescaleFrameBuffer(float width, float height);
	unsigned int getTexture();
	void bind() const;
	void unbind() const;

private:
	unsigned int m_fbo;
	unsigned int m_rbo;
	unsigned int m_texture;
};