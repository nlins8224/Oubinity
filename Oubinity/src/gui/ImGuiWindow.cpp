#include "ImGuiWindow.h"

void ImGuiWindow::update() {
  begin("DEFAULT");
  end();
}

bool ImGuiWindow::begin(std::string name) {
  return ImGui::Begin(name.c_str(), &show);
}
void ImGuiWindow::end() { ImGui::End(); }
