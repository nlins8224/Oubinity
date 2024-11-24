#include "ImGuiSceneWindow.h"

ImGuiSceneWindow::ImGuiSceneWindow(FrameBuffer* frame_buffer)
    : m_scene_fbo{frame_buffer} {}

ImGuiSceneWindow::~ImGuiSceneWindow() { delete m_scene_fbo; }

void ImGuiSceneWindow::update() {
  ImGui::Begin("Scene");
  ImGui::Image((ImTextureID)m_scene_fbo->getTexture(),
               ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
  ImGui::End();
}