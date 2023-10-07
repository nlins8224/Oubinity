#include "GuiLayout.h"

GuiLayout::GuiLayout(ImGuiUIManager* manager, FrameBuffer* scene_buffer)
	: m_manager{manager},
	m_scene_buffer{scene_buffer},
	m_scene_window{scene_buffer},
	m_logs_window{},
	m_settings_window{},
	m_info_window{}
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
	m_manager->addWindow(&m_logs_window);
	m_manager->addWindow(&m_settings_window);
	m_manager->addWindow(&m_info_window);
}
