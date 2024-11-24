#pragma once
#include "../FrameBuffer.h"
#include "GuiSceneWindow.h"
#include "GuiUIManager.h"

class GuiLayout {
 public:
  GuiLayout(GuiUIManager* manager, FrameBuffer* scene_buffer);
  ~GuiLayout();
  void createLayout();

 private:
  GuiUIManager* m_manager;
  FrameBuffer* m_scene_buffer;

  GuiSceneWindow m_scene_window;
};