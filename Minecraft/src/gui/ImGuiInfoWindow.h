#pragma once
#include "ImGuiWindow.h"

class ImGuiInfoWindow : public ImGuiWindow
{
public:
    ImGuiInfoWindow() = default;
    ~ImGuiInfoWindow() = default;
    void update() override;
private:
};