#include "GuiSceneWindow.h"

GuiSceneWindow::GuiSceneWindow(FrameBuffer* frame_buffer)
    : m_scene_fbo{frame_buffer} {}

GuiSceneWindow::~GuiSceneWindow() { delete m_scene_fbo; }

void GuiSceneWindow::update() {
  ImGui::Begin("Scene");
  ImGui::Image((ImTextureID)m_scene_fbo->getTexture(),
               ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
  ImGui::End();
}