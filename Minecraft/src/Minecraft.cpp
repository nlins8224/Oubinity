#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "loguru.hpp"
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
    Camera camera{ glm::vec3(0.0f, 128.0f, 0.0f) };
    TerrainGenerator terrain_generator{ 1337, 1 };
    TextureManager m_texture_manager{ 16, 16, 256 };
    PlayerInput player_input{window.getWindow(), camera};
    MasterRenderer master_renderer(terrain_generator, camera, m_texture_manager.getSkyboxTextureId(), m_texture_manager.getTextureArrayId(), m_texture_manager.getWaterTextureId(), terrain_generator.getWaterHeight(), ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS * CHUNK_SIZE, m_texture_manager.getCloudNoiseId(), glm::ivec2{699, 393 });
    FrameBuffer scene_buffer{ Window::SCREEN_WIDTH, Window::SCREEN_HEIGHT };
    ImGuiUIManager imgui_manager(&window);
    GuiLayout gui_layout{ &imgui_manager, &scene_buffer };
    gui_layout.createLayout();

    SceneShader scene_shader{};
    scene_shader.bind();
    scene_shader.setUniformInt("screenTexture", 0);

    master_renderer.getSkyboxRenderer().getSkyboxLoader().load();
    master_renderer.getGradientRenderer().getGradientLoader().load();
    master_renderer.getWaterRenderer().getWaterLoader().load();
    master_renderer.getSkyRenderer().getSkyLoader().load();
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

    GLint max_ssbo_size;
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &max_ssbo_size);
    LOG_F(INFO, "GL_MAX_SHADER_STORAGE_BLOCK_SIZE is %d bytes", max_ssbo_size);

    //master_renderer.getChunkRenderer().runTraverseSceneInDetachedThread();
    //PreloadedGeneration::parsePNGToHeightMaps("assets/heightmap.png");

    double xpos, ypos;
    while (!glfwWindowShouldClose(window.getWindow()))
    {
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
            LOG_F(INFO, "Player Pos XYZ: (%d, %d, %d)", (int)player_pos.x, (int)player_pos.y, (int)player_pos.z);

            seconds_elapsed += 1.0f;
            frames_per_second = 0;
        }
        glfwGetCursorPos(window.getWindow(), &xpos, &ypos);
        player_input.processInput(delta_time);    
        master_renderer.setTimeElapsed(glfwGetTime());
        master_renderer.setMousePos(glm::vec2(xpos, ypos));
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