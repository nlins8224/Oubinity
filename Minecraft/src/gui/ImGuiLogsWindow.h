#pragma once
#include "ImGuiWindow.h"

class ImGuiLogsWindow : public ImGuiWindow
{
public:
    ImGuiLogsWindow() = default;
    ~ImGuiLogsWindow() = default;
    void update() override;
private:
};