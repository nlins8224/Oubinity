#pragma once
#include <string>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class ImGuiWindow
{
public:
    ImGuiWindow() = default;
    ~ImGuiWindow() = default;
    virtual void update();
protected:
    virtual bool begin(std::string name);
    void end();
    bool show;
};