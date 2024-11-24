#pragma once
#include "../FrameBuffer.h"
#include "GuiWindow.h"

class GuiSceneWindow : public GuiWindow {
 public:
  GuiSceneWindow(FrameBuffer* frame_buffer);
  ~GuiSceneWindow();
  void update() override;

 private:
  FrameBuffer* m_scene_fbo;
};