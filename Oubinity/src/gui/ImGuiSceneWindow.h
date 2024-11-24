#pragma once
#include "../FrameBuffer.h"
#include "ImGuiWindow.h"

class ImGuiSceneWindow : public ImGuiWindow {
 public:
  ImGuiSceneWindow(FrameBuffer* frame_buffer);
  ~ImGuiSceneWindow();
  void update() override;

 private:
  FrameBuffer* m_scene_fbo;
};