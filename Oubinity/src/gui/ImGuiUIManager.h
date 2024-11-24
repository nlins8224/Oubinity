#pragma once
#include <vector>
#include "../FrameBuffer.h"
#include "../io/Window.h"
#include "ImGuiWindow.h"

class ImGuiUIManager {
 public:
  ImGuiUIManager(Window* window);
  ~ImGuiUIManager() = default;
  void update();
  void render();
  void addWindow(ImGuiWindow* newWin);
  void shutDownImGui();

 private:
  std::vector<ImGuiWindow*> m_windows;
  void showDockSpace(bool* p_open);
  void setstyle();
  void updateWindows();
};