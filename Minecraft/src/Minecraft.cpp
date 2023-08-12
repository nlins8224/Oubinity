#pragma once
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

const int scr_width = 1200;
const int scr_height = 1600;


float fov = 90.0f;

int main()
{
    Window window{"Voxel Engine"};
    window.windowInit();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << glGetError() << std::endl;
    Camera camera{ glm::vec3(156.0f, 128.0f, 3.0f) };
    TerrainGenerator terrain_generator{ 1337, 4, 7 };
    TextureManager m_texture_manager{ 16, 16, 256 };
    PlayerInput player_input{window.getWindow(), camera};
    MasterRenderer master_renderer(camera, m_texture_manager.getSkyboxTextureId(), m_texture_manager.getTextureArrayId());

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
        std::cerr << "ERROR: Minimum OpenGL version required for this demo is 4.6. Your current version is " << major << "." << minor << std::endl;
        exit(-1);
    }


    while (!glfwWindowShouldClose(window.getWindow()))
    {
        OPTICK_FRAME("MainThread");
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        frames_per_second++;
        // Once per second
        int block_amount = 1;
        if (current_frame - seconds_elapsed >= 1.0f)
        {
            glm::vec3 player_pos = player_input.getCamera().getCameraPos();

            std::cout << "FPS: " << frames_per_second << std::endl;
            std::cout << "PLAYER POS XZ: " << player_pos.x << " " << player_pos.z << std::endl;

            seconds_elapsed += 1.0f;
            frames_per_second = 0;
        }

        player_input.processInput(delta_time);       
        master_renderer.clear();
        master_renderer.render(camera);
        glfwSwapBuffers(window.getWindow());
        glfwPollEvents();
    }

    return 0;
}

