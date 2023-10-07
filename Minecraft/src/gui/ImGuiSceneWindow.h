#pragma once
#include "ImGuiWindow.h"
#include "../FrameBuffer.h"

class ImGuiSceneWindow : public ImGuiWindow
{
public:
    ImGuiSceneWindow(FrameBuffer* frame_buffer);
    ~ImGuiSceneWindow();
    void update() override;
private:
    FrameBuffer* m_scene_fbo;
};