#pragma once
#include "ImGuiWindow.h"

class ImGuiSettingsWindow : public ImGuiWindow
{
public:
    ImGuiSettingsWindow() = default;
    ~ImGuiSettingsWindow() = default;
    void update() override;
private:
};