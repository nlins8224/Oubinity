#pragma once
#include "ImGuiSceneWindow.h"
#include "ImGuiSettingsWindow.h"
#include "ImGuiInfoWindow.h"
#include "ImGuiLogsWindow.h"
#include "ImGuiUIManager.h"
#include "../FrameBuffer.h"

class GuiLayout
{

public:
    GuiLayout(ImGuiUIManager* manager, FrameBuffer* scene_buffer);
    ~GuiLayout();
    void createLayout();

private:
    ImGuiUIManager* m_manager;
    FrameBuffer* m_scene_buffer;

    ImGuiSceneWindow m_scene_window;
    ImGuiLogsWindow m_logs_window;
    ImGuiSettingsWindow m_settings_window;
    ImGuiInfoWindow m_info_window;
};