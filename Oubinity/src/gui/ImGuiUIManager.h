#pragma once
#include <vector>
#include "ImGuiWindow.h"
#include "../io/Window.h"
#include "../FrameBuffer.h"

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