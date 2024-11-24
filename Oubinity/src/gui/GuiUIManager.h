#pragma once
#include <vector>
#include "../FrameBuffer.h"
#include "../io/Window.h"
#include "GuiWindow.h"

class GuiUIManager {
 public:
  GuiUIManager(Window* window);
  ~GuiUIManager() = default;
  void update();
  void render();
  void addWindow(GuiWindow* newWin);
  void shutDownImGui();

 private:
  std::vector<GuiWindow*> m_windows;
  void showDockSpace(bool* p_open);
  void setstyle();
  void updateWindows();
};