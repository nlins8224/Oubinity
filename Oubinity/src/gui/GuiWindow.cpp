#include "GuiWindow.h"

void GuiWindow::update() {
  begin("DEFAULT");
  end();
}

bool GuiWindow::begin(std::string name) {
  return ImGui::Begin(name.c_str(), &show);
}
void GuiWindow::end() { ImGui::End(); }
