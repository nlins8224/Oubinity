#include "GuiLayout.h"

GuiLayout::GuiLayout(ImGuiUIManager* manager, FrameBuffer* scene_buffer)
	: m_manager{manager},
	m_scene_buffer{scene_buffer},
	m_scene_window{scene_buffer}
{
}

GuiLayout::~GuiLayout()
{
	delete m_manager;
	delete m_scene_buffer;
}

void GuiLayout::createLayout()
{
	m_manager->addWindow(&m_scene_window);
}
