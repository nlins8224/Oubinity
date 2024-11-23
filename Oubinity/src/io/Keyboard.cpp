#include "Keyboard.h"

bool Keyboard::m_keys[GLFW_KEY_LAST]{ 0 };
bool Keyboard::m_keys_changed[GLFW_KEY_LAST]{ 0 };

void Keyboard::keyStateCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_RELEASE)
	{
		if (!m_keys[key])
		{
			m_keys[key] = true;
		}
	}
	else
	{
		m_keys[key] = false;
	}

	m_keys_changed[key] = action != GLFW_REPEAT;
}
bool Keyboard::key(int key)
{
	return m_keys[key];
}
bool Keyboard::keyChanged(int key)
{
	bool ret = m_keys_changed[key];
	m_keys_changed[key] = false;
	return ret;
}
bool Keyboard::keyWentUp(int key)
{
	return !m_keys[key] && keyChanged(key);
}
bool Keyboard::keyWentDown(int key)
{
	return m_keys[key] && keyChanged(key);
}