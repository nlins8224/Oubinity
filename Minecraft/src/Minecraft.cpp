#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "loguru.hpp"
#include "optick.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>

#include "terrain_generation/TerrainGenerator.h"
#include "PlayerInput.h"
#include "io/Window.h"
#include "shader/Shader.h"
#include "Camera.h"
#include "renderer/MasterRenderer.h"
#include "TextureManager.h"
#include "FrameBuffer.h"
#include "shader/SceneShader.h"
#include "gui/ImGuiUIManager.h"
#include "gui/GuiLayout.h"

int main()
{
    Window window{"Oubinity Engine"};
    window.windowInit();
  
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_F(ERROR, "Failed to initialize GLAD");
        return -1;
    }

    std::cout << glGetError() << std::endl;
    Camera camera{ glm::vec3(156.0f, 128.0f, 3.0f) };
    TerrainGenerator terrain_generator{ 1337, 4, 7 };
    TextureManager m_texture_manager{ 16, 16, 256 };
    PlayerInput player_input{window.getWindow(), camera};
    MasterRenderer master_renderer(camera, m_texture_manager.getSkyboxTextureId(), m_texture_manager.getTextureArrayId());
    FrameBuffer scene_buffer{ Window::SCREEN_WIDTH, Window::SCREEN_HEIGHT };
    ImGuiUIManager imgui_manager(&window);
    GuiLayout gui_layout{ &imgui_manager, &scene_buffer };
    gui_layout.createLayout();

    SceneShader scene_shader{};
    scene_shader.bind();
    scene_shader.setUniformInt("screenTexture", 0);

    master_renderer.getSkyboxRenderer().getSkyboxLoader().load();
    master_renderer.getGradientRenderer().getGradientLoader().load();
    master_renderer.initConfig();
    glfwSetWindowUserPointer(window.getWindow(), &player_input);
    glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    float delta_time = 0.0f;
    float last_frame = 0.0f;
    float seconds_elapsed = 0.0f;
    int frames_per_second = 0;

    glm::ivec3 current_block_pos{ 0, 0, 0 };

    const GLubyte* glVersion(glGetString(GL_VERSION));
    int major = glVersion[0] - '0';
    int minor = glVersion[2] - '0';
    if (major < 4 || minor < 6)
    {
        LOG_F(ERROR, "Minimum OpenGL version required for this demo is 4.6.Your current version is %d.%d", major, minor);
        exit(-1);
    }

    //master_renderer.getChunkRenderer().runTraverseSceneInDetachedThread();
    while (!glfwWindowShouldClose(window.getWindow()))
    {
        OPTICK_FRAME("MainThread");
        scene_buffer.bind();

        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        frames_per_second++;
        // Once per second
        int block_amount = 1;
        if (current_frame - seconds_elapsed >= 1.0f)
        {
            glm::vec3 player_pos = player_input.getCamera().getCameraPos();

            LOG_F(INFO, "FPS: %d", frames_per_second);
            LOG_F(INFO, "Player Pos XZ: (%d, %d)", (int)player_pos.x / CHUNK_SIZE, (int)player_pos.z / CHUNK_SIZE);

            seconds_elapsed += 1.0f;
            frames_per_second = 0;
        }

        player_input.processInput(delta_time);       
        master_renderer.clear();
        master_renderer.render(camera);

        scene_buffer.unbind();
        imgui_manager.update();
        imgui_manager.render();

        glfwSwapBuffers(window.getWindow());
        glfwPollEvents();

    }
    imgui_manager.shutDownImGui();
    return 0;
}