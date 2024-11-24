#pragma once
#include "../FrameBuffer.h"
#include "ImGuiSceneWindow.h"
#include "ImGuiUIManager.h"

class GuiLayout {
 public:
  GuiLayout(ImGuiUIManager* manager, FrameBuffer* scene_buffer);
  ~GuiLayout();
  void createLayout();

 private:
  ImGuiUIManager* m_manager;
  FrameBuffer* m_scene_buffer;

  ImGuiSceneWindow m_scene_window;
};