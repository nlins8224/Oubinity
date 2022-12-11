#pragma once
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
class MasterRenderer
{
public:
	MasterRenderer() = default;
	~MasterRenderer() = default;
	void initConfig() const;
	void clear() const;
};

